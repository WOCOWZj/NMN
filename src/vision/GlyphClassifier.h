#pragma once

#include <QString>

#include <opencv2/core.hpp>

namespace tesseract {
class TessBaseAPI;
}

namespace vision {

/// 送入分类器前对单个字形的归一化参数。
/// 之所以做成参数而不是硬编码：不同字号/字体下，留白与缩放高度对
/// LSTM 的影响差异很大，需要实测筛选。
struct GlyphStyle
{
    int padding = 3;        ///< 四周补白像素
    int targetHeight = 48;  ///< 放大到的字高；0 表示不缩放
    bool invert = true;     ///< 墨迹 255 → 黑字白底
};

/// 单字形分类器。链接 libtesseract，不走子进程。
class GlyphClassifier
{
public:
    explicit GlyphClassifier(QString tessdataPath = QStringLiteral("D:/Tesseract-OCR/tessdata"));
    ~GlyphClassifier();

    GlyphClassifier(const GlyphClassifier&) = delete;
    GlyphClassifier& operator=(const GlyphClassifier&) = delete;

    bool initialize(QString* error = nullptr);
    bool isReady() const { return m_api != nullptr; }
    const QString& tessdataPath() const { return m_tessdataPath; }
    void setTessdataPath(const QString& path);

    /// 识别单个字形。返回 1..7，0 表示未能判定。
    /// ink 必须是本层约定的"墨迹=255"二值图。
    int classify(const cv::Mat& ink, const cv::Rect& box, const GlyphStyle& style = GlyphStyle());

    /// 纯几何兜底：简谱里最瘦的字形只有 "1"（细高单柱）。
    /// 当 OCR 对细窄字形返回空串时用它兜底。
    /// 判据不能只看长宽比——左括号 "(" 同样细长，但它的墨迹沿曲线分布，
    /// 不存在贯穿全高的实心竖列；而 "1" 的主笔正是这样一列。
    static int geometricGuess(const cv::Mat& ink, const cv::Rect& box);

private:
    void release();
    cv::Mat prepare(const cv::Mat& ink, const cv::Rect& box, const GlyphStyle& style) const;

    QString m_tessdataPath;
    tesseract::TessBaseAPI* m_api = nullptr;
};

} // namespace vision
