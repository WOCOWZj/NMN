#include "vision/Segmenter.h"

#include <QtGlobal>

#include <opencv2/imgproc.hpp>
#include <vector>

namespace vision {

namespace {
constexpr double kRowOccupancy = 0.15;  // 行切分：一行内非零像素占比阈值
constexpr double kCellOccupancy = 0.20; // 格切分：一格内非零像素占比阈值
constexpr int kMinRunLength = 5;        // 连续有效像素的最小长度
}

QList<LineBox> Segmenter::splitRows(const cv::Mat& binary)
{
    QList<LineBox> lines;
    if (binary.empty())
        return lines;

    const int rows = binary.rows;
    const int cols = binary.cols;

    std::vector<int> projection(static_cast<size_t>(rows), 0);
    for (int i = 0; i < rows; ++i)
        projection[static_cast<size_t>(i)] = cv::countNonZero(binary.row(i));

    for (int i = 0; i < rows; ++i)
    {
        int j = 0;
        while (i + j < rows && projection[static_cast<size_t>(i + j)] > cols * kRowOccupancy)
            ++j;

        if (j > kMinRunLength)
        {
            LineBox box;
            box.top = qMax(0, i - j / 4);
            // 与原实现一致：高度取 min(1.5*j, rows-i)，再做一次上界保护
            box.height = qMin(qMin(j / 2 * 3, rows - i), rows - box.top);
            if (box.height > 0)
                lines.append(box);
        }

        i += j;
    }

    return lines;
}

QList<QRect> Segmenter::splitCells(const cv::Mat& binary, const QList<LineBox>& lines)
{
    QList<QRect> cells;
    if (binary.empty())
        return cells;

    const int cols = binary.cols;

    for (const LineBox& line : lines)
    {
        const int top = qBound(0, line.top, binary.rows);
        const int bottom = qBound(top, line.top + line.height, binary.rows);
        const int height = bottom - top;
        if (height <= 0)
            continue;

        std::vector<int> projection(static_cast<size_t>(cols), 0);
        const cv::Mat band = binary.rowRange(top, bottom);
        for (int i = 0; i < cols; ++i)
            projection[static_cast<size_t>(i)] = cv::countNonZero(band.col(i));

        for (int i = 0; i < cols; ++i)
        {
            int j = 0;
            while (i + j < cols && projection[static_cast<size_t>(i + j)] > height * kCellOccupancy)
                ++j;

            if (j > kMinRunLength)
            {
                const int left = qMax(0, i - j / 4);
                const int width = qMin(cols - left, j * 3 / 2);
                if (width > 0)
                    cells.append(QRect(left, top, width, height));
            }

            i += j;
        }
    }

    return cells;
}

} // namespace vision
