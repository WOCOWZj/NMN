#pragma once

#include <QList>
#include <QRect>

#include <opencv2/core.hpp>

namespace vision {

/// 一行乐谱的纵向范围
struct LineBox
{
    int top = 0;
    int height = 0;
};

/// 基于投影的版面切分：先切行，再在行内切音符格子。
/// 阈值与原 imgprcs.cpp 保持一致，切分结果不变。
class Segmenter
{
public:
    static QList<LineBox> splitRows(const cv::Mat& binary);
    static QList<QRect> splitCells(const cv::Mat& binary, const QList<LineBox>& lines);
};

} // namespace vision
