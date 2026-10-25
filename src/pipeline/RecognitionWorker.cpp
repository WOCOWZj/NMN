#include "pipeline/RecognitionWorker.h"

#include "util/Logger.h"
#include "vision/Preprocessor.h"
#include "vision/Segmenter.h"

#include <QtGlobal>

#include <opencv2/imgproc.hpp>

#include <algorithm>

namespace pipeline {

RecognitionWorker::RecognitionWorker(QObject* parent)
    : QObject(parent)
{
}

cv::Mat RecognitionWorker::toBgrMat(const QImage& image)
{
    if (image.isNull())
        return cv::Mat();

    // 整个流程只在这里把 QImage 转成 cv::Mat 一次，
    // 取代原先 QPixmap 判空、cv::imread、QPixmap 重读的三趟 IO。
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

    util::Logger::info(QStringLiteral("开始识别：%1 x %2").arg(image.width()).arg(image.height()));

    emit progress(QStringLiteral("正在预处理…"));
    const cv::Mat binary = vision::Preprocessor::prepare(bgr);

    emit progress(QStringLiteral("正在切分行…"));
    const QList<vision::LineBox> lines = vision::Segmenter::splitRows(binary);
    util::Logger::info(QStringLiteral("切分出 %1 行").arg(lines.size()));
    if (lines.isEmpty())
    {
        emit failed(QStringLiteral("未检测到任何乐谱行，请确认图片包含简谱或五线谱"));
        return;
    }

    emit progress(QStringLiteral("正在切分音符…"));
    const QList<QRect> cells = vision::Segmenter::splitCells(binary, lines);
    util::Logger::info(QStringLiteral("切分出 %1 个候选区域").arg(cells.size()));
    if (cells.isEmpty())
    {
        emit failed(QStringLiteral("未检测到任何音符候选区域"));
        return;
    }

    emit progress(QStringLiteral("正在识别 %1 个区域…").arg(cells.size()));

    QString ocrError;
    const QList<QString> texts = m_engine.recognize(binary, cells, &ocrError);
    if (texts.isEmpty())
    {
        emit failed(ocrError.isEmpty() ? QStringLiteral("OCR 未能返回结果") : ocrError);
        return;
    }

    QList<core::Note> notes;
    for (int i = 0; i < texts.size() && i < cells.size(); ++i)
    {
        const QString& text = texts.at(i);
        if (text.isEmpty())
            continue;

        const QRect& cell = cells.at(i);
        const QList<QRect> boxes = splitGlyphBoxes(binary, cell, static_cast<int>(text.size()));

        for (int k = 0; k < text.size(); ++k)
        {
            const QChar ch = text.at(k);
            if (!ch.isDigit())
                continue;

            // '0' 是 Tesseract 的占位符，跳过
            if (ch == QLatin1Char('0'))
                continue;

            const int index = ch.digitValue() - 1;
            if (index < 0 || index > 6)
                continue;

            core::Note note;
            note.name = static_cast<core::Note::Name>(index);
            note.rect = (k < boxes.size()) ? boxes.at(k) : cell;
            notes.append(note);
        }
    }

    if (notes.isEmpty())
    {
        emit failed(QStringLiteral("未识别出任何音符"));
        return;
    }

    core::Score score;
    score.setNotes(notes);
    util::Logger::info(QStringLiteral("识别出 %1 个音符").arg(score.size()));

    emit finished(score);
}

QList<QRect> RecognitionWorker::splitGlyphBoxes(const cv::Mat& binary,
                                                const QRect& cell,
                                                int expectedCount)
{
    QList<QRect> boxes;
    const QRect clipped = cell.intersected(QRect(0, 0, binary.cols, binary.rows));
    if (clipped.width() <= 0 || clipped.height() <= 0)
        return boxes;

    if (expectedCount <= 1)
    {
        boxes.append(clipped);
        return boxes;
    }

    const cv::Mat roi = binary(cv::Rect(clipped.x(), clipped.y(), clipped.width(), clipped.height()));

    cv::Mat labels;
    cv::Mat stats;
    cv::Mat centroids;
    const int componentCount =
        cv::connectedComponentsWithStats(roi, labels, stats, centroids, 8, CV_32S);

    // 过滤掉乐谱线残留等小噪点
    const int minArea = qMax(4, clipped.height() * 2);

    QList<QRect> parts;
    for (int i = 1; i < componentCount; ++i)
    {
        if (stats.at<int>(i, cv::CC_STAT_AREA) < minArea)
            continue;

        parts.append(QRect(stats.at<int>(i, cv::CC_STAT_LEFT),
                           stats.at<int>(i, cv::CC_STAT_TOP),
                           stats.at<int>(i, cv::CC_STAT_WIDTH),
                           stats.at<int>(i, cv::CC_STAT_HEIGHT)));
    }

    if (parts.size() != expectedCount)
    {
        // 修复前的行为：按字符数均分矩形宽度。
        // 连通域数量与 OCR 字符数对不上时退回这里，不影响旧有识别结果。
        const int width = clipped.width() / expectedCount;
        for (int k = 0; k < expectedCount; ++k)
        {
            const int left = clipped.x() + width * k;
            const int w = (k == expectedCount - 1) ? clipped.right() - left + 1 : width;
            boxes.append(QRect(left, clipped.y(), qMax(1, w), clipped.height()));
        }
        return boxes;
    }

    std::sort(parts.begin(), parts.end(), [](const QRect& a, const QRect& b) {
        return a.x() < b.x();
    });

    for (const QRect& part : parts)
        boxes.append(part.translated(clipped.topLeft()));

    return boxes;
}

} // namespace pipeline
