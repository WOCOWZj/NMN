#include "vision/ImageNormalizer.h"

#include <opencv2/imgproc.hpp>

namespace vision {

namespace {
constexpr int kBlockSize = 21;
constexpr double kC = 12.0;
constexpr double kMinInkRatio = 0.0005;
constexpr double kMaxInkRatio = 0.5;
}

cv::Mat ImageNormalizer::binarize(const cv::Mat& src)
{
    if (src.empty())
        return cv::Mat();

    cv::Mat gray;
    if (src.channels() == 4)
        cv::cvtColor(src, gray, cv::COLOR_BGRA2GRAY);
    else if (src.channels() == 3)
        cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    else if (src.channels() == 1)
        gray = src;
    else
        return cv::Mat();

    cv::Mat blurred;
    cv::GaussianBlur(gray, blurred, cv::Size(3, 3), 0.5);

    // 自适应阈值能应对扫描阴影与光照不均。
    // THRESH_BINARY_INV 的语义是"暗部置 255"，即墨迹=255、背景=0，符合本层约定。
    cv::Mat ink;
    cv::adaptiveThreshold(blurred,
                          ink,
                          255,
                          cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                          cv::THRESH_BINARY_INV,
                          kBlockSize,
                          kC);

    const double total = static_cast<double>(ink.rows) * ink.cols;
    const double ratio = cv::countNonZero(ink) / total;

    // 低对比度、纯色背景或极端缩放时自适应阈值会失控，退回 Otsu
    if (ratio < kMinInkRatio || ratio > kMaxInkRatio)
    {
        cv::Mat otsu;
        cv::threshold(blurred, otsu, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
        const double otsuRatio = cv::countNonZero(otsu) / total;
        if (otsuRatio > kMinInkRatio && otsuRatio < kMaxInkRatio)
            ink = otsu;
    }

    return ink;
}

cv::Rect ImageNormalizer::contentBounds(const cv::Mat& ink)
{
    if (ink.empty())
        return cv::Rect();

    cv::Mat points;
    cv::findNonZero(ink, points);
    if (points.empty())
        return cv::Rect();
    return cv::boundingRect(points);
}

} // namespace vision
