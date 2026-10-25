#include "pipeline/RecognitionWorker.h"

#include "util/Logger.h"
#include "vision/GlyphExtractor.h"
#include "vision/ImageNormalizer.h"

#include <QStringList>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <cmath>

namespace pipeline {

namespace {

/// 字形太小时二值化后噪声会打碎闭合回路，模板匹配与洞数统计都会失准。
/// 把字形放大到这个高度再处理，实测可让识别率从 25/29 提升到 28/29。
constexpr double kTargetGlyphHeight = 40.0;
constexpr double kMaxUpscale = 3.0;
constexpr double kOcrFallbackThreshold = 0.70;

core::Note::OctaveDot toOctaveDot(vision::Octave octave)
{
    switch (octave)
    {
    case vision::Octave::Upper:
        return core::Note::OctaveDot::Upper;
    case vision::Octave::Lower:
        return core::Note::OctaveDot::Lower;
    case vision::Octave::None:
    default:
        return core::Note::OctaveDot::None;
    }
}

struct Analysis
{
    bool ok = false;
    QString error;

    std::vector<vision::GlyphBox> glyphs;
    cv::Mat cleaned;
    int rowCount = 0;
    double medianGlyphHeight = 0.0;
    double scale = 1.0;

    QList<core::Note> notes;
    QStringList digits;
    int unrecognized = 0;
    int ocrFallbacks = 0;
};

/// 在给定缩放比例下完整跑一遍分割与分类
Analysis analyzeAt(const cv::Mat& bgr,
                   double scale,
                   const vision::TemplateClassifier& templates,
                   vision::GlyphClassifier* ocrFallback)
{
    Analysis result;
    result.scale = scale;

    cv::Mat working;
    if (scale > 1.01)
        cv::resize(bgr, working, cv::Size(), scale, scale, cv::INTER_CUBIC);
    else
        working = bgr;

    const cv::Mat ink = vision::ImageNormalizer::binarize(working);
    if (ink.empty())
    {
        result.error = QStringLiteral("二值化失败");
        return result;
    }

    const cv::Mat cleaned = vision::GlyphExtractor::suppressStaffLines(ink);
    const std::vector<vision::RowBand> rows =
        vision::GlyphExtractor::selectMusicRows(cleaned, vision::GlyphExtractor::detectRows(cleaned));

    result.rowCount = static_cast<int>(rows.size());
    result.cleaned = cleaned;

    if (rows.empty())
    {
        result.error = QStringLiteral("未检测到乐谱行");
        return result;
    }

    std::vector<int> heights;

    for (const vision::RowBand& band : rows)
    {
        const vision::Extraction row = vision::GlyphExtractor::extractRow(cleaned, band);
        for (const vision::GlyphBox& glyph : row.glyphs)
        {
            result.glyphs.push_back(glyph);
            heights.push_back(glyph.box.height);
        }
    }

    if (result.glyphs.empty())
    {
        result.error = QStringLiteral("未检测到音符候选字形");
        return result;
    }

    std::sort(heights.begin(), heights.end());
    result.medianGlyphHeight = heights[heights.size() / 2];

    // 八度点归属（在提取阶段需要 marks，此处重建一次）
    vision::Extraction merge;
    merge.glyphs = result.glyphs;
    for (const vision::RowBand& band : rows)
    {
        const vision::Extraction row = vision::GlyphExtractor::extractRow(cleaned, band);
        for (const vision::MarkBox& mark : row.marks)
            merge.marks.push_back(mark);
    }
    vision::GlyphExtractor::associateOctaves(merge);
    result.glyphs = merge.glyphs;

    for (const vision::GlyphBox& glyph : result.glyphs)
    {
        const cv::Mat mask = cleaned(glyph.box);

        vision::TemplateMatch match = templates.classify(mask);
        int digit = match.digit;

        // 置信度不足时用 Tesseract 复核一次
        if (match.score < kOcrFallbackThreshold && ocrFallback && ocrFallback->isReady())
        {
            const int fallback = ocrFallback->classify(cleaned, glyph.box);
            if (fallback > 0)
            {
                digit = fallback;
                ++result.ocrFallbacks;
            }
        }

        if (digit <= 0)
        {
            ++result.unrecognized;
            continue;
        }

        core::Note note;
        note.name = static_cast<core::Note::Name>(digit - 1);
        note.rect = QRect(static_cast<int>(std::lround(glyph.box.x / scale)),
                          static_cast<int>(std::lround(glyph.box.y / scale)),
                          static_cast<int>(std::lround(glyph.box.width / scale)),
                          static_cast<int>(std::lround(glyph.box.height / scale)));
        note.octaveDot = toOctaveDot(glyph.octave);
        result.notes.append(note);
        result.digits << QString::number(digit);
    }

    result.ok = true;
    return result;
}

} // namespace

RecognitionWorker::RecognitionWorker(QObject* parent)
    : QObject(parent)
{
}

cv::Mat RecognitionWorker::toBgrMat(const QImage& image)
{
    if (image.isNull())
        return cv::Mat();

    QImage buffer = image.convertToFormat(QImage::Format_ARGB32);
    const cv::Mat view(buffer.height(),
                       buffer.width(),
                       CV_8UC4,
                       buffer.bits(),
                       static_cast<size_t>(buffer.bytesPerLine()));

    cv::Mat bgr;
    cv::cvtColor(view, bgr, cv::COLOR_BGRA2BGR);
    return bgr.clone();
}

void RecognitionWorker::run(const QImage& image)
{
    if (image.isNull())
    {
        emit failed(QStringLiteral("图像为空"));
        return;
    }

    const cv::Mat bgr = toBgrMat(image);
    if (bgr.empty())
    {
        emit failed(QStringLiteral("图像转换失败"));
        return;
    }

    if (!m_templatesLoaded)
    {
        emit progress(QStringLiteral("正在加载字形模板…"));
        QString templateError;
        if (!m_templates.load(QStringLiteral(":/templates"), &templateError))
        {
            emit failed(templateError);
            return;
        }
        m_templatesLoaded = true;
        util::Logger::info(QStringLiteral("已加载 %1 个字形模板").arg(m_templates.size()));
    }

    // Tesseract 仅作低置信度兜底，加载失败不影响主流程
    if (!m_ocrFallback.isReady())
    {
        QString ocrError;
        if (!m_ocrFallback.initialize(&ocrError))
            util::Logger::warn(QStringLiteral("Tesseract 兜底不可用：%1").arg(ocrError));
    }

    util::Logger::info(QStringLiteral("开始识别：%1 x %2").arg(image.width()).arg(image.height()));

    // 第一遍：原始分辨率，只为估计字号
    emit progress(QStringLiteral("正在分析版面…"));
    Analysis probe = analyzeAt(bgr, 1.0, m_templates, nullptr);

    double scale = 1.0;
    if (probe.ok && probe.medianGlyphHeight > 0 && probe.medianGlyphHeight < kTargetGlyphHeight)
    {
        scale = kTargetGlyphHeight / probe.medianGlyphHeight;
        scale = std::min(kMaxUpscale, std::max(1.0, scale));
        scale = std::round(scale * 4.0) / 4.0; // 取 0.25 步进
    }

    Analysis result;
    if (scale > 1.01)
    {
        emit progress(QStringLiteral("正在放大 %1 倍重新切分…").arg(scale, 0, 'f', 2));
        result = analyzeAt(bgr, scale, m_templates, &m_ocrFallback);
        if (!result.ok)
            result = probe; // 放大反而失败时退回第一遍结果
    }
    else
    {
        result = analyzeAt(bgr, 1.0, m_templates, &m_ocrFallback);
        if (!result.ok)
            result = probe;
    }

    if (!result.ok)
    {
        emit failed(result.error.isEmpty() ? QStringLiteral("识别失败") : result.error);
        return;
    }

    util::Logger::info(QStringLiteral("切分：%1 行，%2 个字形，中位字高 %3 px，缩放 %4，模板 %5，OCR 兜底 %6")
                           .arg(result.rowCount)
                           .arg(result.glyphs.size())
                           .arg(result.medianGlyphHeight, 0, 'f', 1)
                           .arg(result.scale, 0, 'f', 2)
                           .arg(m_templates.size())
                           .arg(result.ocrFallbacks));

    if (result.notes.isEmpty())
    {
        emit failed(QStringLiteral("未识别出任何音符"));
        return;
    }

    core::Score score;
    score.setNotes(result.notes);

    util::Logger::info(QStringLiteral("识别出 %1 个音符（未判定 %2 个）：%3")
                           .arg(score.size())
                           .arg(result.unrecognized)
                           .arg(result.digits.join(QLatin1Char(' '))));

    if (qEnvironmentVariableIsSet("NMN_DEBUG_SEG") && !result.cleaned.empty())
    {
        cv::Mat canvas;
        cv::cvtColor(result.cleaned, canvas, cv::COLOR_GRAY2BGR);
        for (const vision::GlyphBox& glyph : result.glyphs)
        {
            cv::rectangle(canvas,
                          cv::Rect(glyph.box.x, glyph.box.y, glyph.box.width, glyph.box.height),
                          cv::Scalar(0, 0, 255),
                          1);
        }
        cv::imwrite("assets/seg_debug.png", canvas);
        util::Logger::info(QStringLiteral("调试图已保存：assets/seg_debug.png"));
    }

    emit finished(score);
}

} // namespace pipeline
