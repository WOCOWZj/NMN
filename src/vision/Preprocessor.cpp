#include "vision/Preprocessor.h"

#include <opencv2/imgproc.hpp>

namespace vision {

cv::Mat Preprocessor::prepare(const cv::Mat& bgr)
{
    if (bgr.empty())
        return cv::Mat();

    cv::Mat gray;
    if (bgr.channels() == 1)
        gray = bgr;
    else
        // 修复：原实现用 COLOR_RGB2GRAY 处理 imread 得到的 BGR 数据，
        // 红蓝通道权重被对调，灰度值系统性偏移。
        cv::cvtColor(bgr, gray, cv::COLOR_BGR2GRAY);

    cv::Mat blurred;
    cv::GaussianBlur(gray, blurred, cv::Size(3, 3), 0.5);

    cv::Mat binary;
    constexpr int kBlockSize = 21;
    constexpr double kC = 12.0;
    cv::adaptiveThreshold(blurred,
                          binary,
                          255,
                          cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                          cv::THRESH_BINARY_INV,
                          kBlockSize,
                          kC);

    return binary;
}

} // namespace vision
