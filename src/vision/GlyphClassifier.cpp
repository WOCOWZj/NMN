#include "vision/GlyphClassifier.h"

#include <QDir>

#include <tesseract/baseapi.h>

#include <opencv2/imgproc.hpp>

namespace vision {

GlyphClassifier::GlyphClassifier(QString tessdataPath)
    : m_tessdataPath(std::move(tessdataPath))
{
}

GlyphClassifier::~GlyphClassifier()
{
    release();
}

void GlyphClassifier::setTessdataPath(const QString& path)
{
    if (path == m_tessdataPath)
        return;
    m_tessdataPath = path;
    release();
}

void GlyphClassifier::release()
{
    if (!m_api)
        return;
    m_api->End();
    delete m_api;
    m_api = nullptr;
}

bool GlyphClassifier::initialize(QString* error)
{
    if (m_api)
        return true;

    if (!QDir(m_tessdataPath).exists())
    {
        if (error)
            *error = QStringLiteral("找不到 tessdata 目录：%1").arg(m_tessdataPath);
        return false;
    }

    m_api = new tesseract::TessBaseAPI();

    const QByteArray dataPath = m_tessdataPath.toUtf8();
    if (m_api->Init(dataPath.constData(), "eng", tesseract::OEM_DEFAULT) != 0)
    {
        if (error)
            *error = QStringLiteral("Tesseract 初始化失败：%1").arg(m_tessdataPath);
        release();
        return false;
    }

    m_api->SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
    m_api->SetVariable("tessedit_char_whitelist", "1234567");
    return true;
}

cv::Mat GlyphClassifier::prepare(const cv::Mat& ink, const cv::Rect& box, const GlyphStyle& style) const
{
    const cv::Rect bounds(0, 0, ink.cols, ink.rows);
    const cv::Rect clipped = box & bounds;
    if (clipped.width <= 0 || clipped.height <= 0)
        return cv::Mat();

    cv::Mat glyph = ink(clipped).clone();

    // 本层约定墨迹=255，而 Tesseract 要黑字白底
    if (style.invert)
        cv::bitwise_not(glyph, glyph);

    if (style.padding > 0)
    {
        const cv::Scalar border = style.invert ? cv::Scalar(255) : cv::Scalar(0);
        cv::copyMakeBorder(glyph,
                           glyph,
                           style.padding,
                           style.padding,
                           style.padding,
                           style.padding,
                           cv::BORDER_CONSTANT,
                           border);
    }

    if (style.targetHeight > 0 && glyph.rows < style.targetHeight)
    {
        const double scale = static_cast<double>(style.targetHeight) / glyph.rows;
        cv::resize(glyph, glyph, cv::Size(), scale, scale, cv::INTER_LINEAR);
    }

    return glyph;
}

int GlyphClassifier::classify(const cv::Mat& ink, const cv::Rect& box, const GlyphStyle& style)
{
    const cv::Mat glyph = prepare(ink, box, style);
    if (glyph.empty())
        return 0;

    int digit = 0;

    if (m_api)
    {
        m_api->SetImage(glyph.data,
                        glyph.cols,
                        glyph.rows,
                        glyph.channels(),
                        static_cast<int>(glyph.step));

        char* raw = m_api->GetUTF8Text();
        if (raw)
        {
            const QString text = QString::fromUtf8(raw);
            delete[] raw;

            for (const QChar ch : text)
            {
                if (!ch.isDigit())
                    continue;
                const int value = ch.digitValue();
                if (value >= 1 && value <= 7)
                {
                    digit = value;
                    break;
                }
            }
        }
    }

    if (digit == 0)
        digit = geometricGuess(ink, box);

    return digit;
}

int GlyphClassifier::geometricGuess(const cv::Mat& ink, const cv::Rect& box)
{
    if (box.height <= 0 || ink.empty())
        return 0;

    const double aspect = static_cast<double>(box.width) / box.height;
    if (aspect >= 0.45)
        return 0;

    const cv::Rect bounds(0, 0, ink.cols, ink.rows);
    const cv::Rect clipped = box & bounds;
    if (clipped.width <= 0 || clipped.height <= 0)
        return 0;

    const cv::Mat glyph = ink(clipped);

    // 找出墨量最集中的一列，看它是否几乎贯穿整个字高
    int bestColumn = 0;
    for (int x = 0; x < glyph.cols; ++x)
        bestColumn = std::max(bestColumn, cv::countNonZero(glyph.col(x)));

    const double stemRatio = static_cast<double>(bestColumn) / glyph.rows;

    // "1" 有一根接近满高的实心竖笔；"(" 之类的曲线没有
    return (stemRatio >= 0.8) ? 1 : 0;
}

} // namespace vision
