#pragma once

#include <opencv2/core.hpp>

namespace vision {

/// 图像预处理：灰度 → 高斯去噪 → 自适应二值化（反相，笔画为白）
class Preprocessor
{
public:
    static cv::Mat prepare(const cv::Mat& bgr);
};

} // namespace vision
