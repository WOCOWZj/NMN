#pragma once

#include <opencv2/core.hpp>

namespace vision {

/// 输入规范化：任意图像 → 统一极性的二值墨迹图。
/// 约定：墨迹（笔画）= 255，背景 = 0。整个 vision 层都遵循这一约定。
class ImageNormalizer
{
public:
    /// 灰度化 + 自适应阈值。对比度极端时自动退回 Otsu。
    static cv::Mat binarize(const cv::Mat& src);

    /// 墨迹外接矩形，用于裁掉大片空白
    static cv::Rect contentBounds(const cv::Mat& ink);
};

} // namespace vision
