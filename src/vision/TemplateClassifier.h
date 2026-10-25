#pragma once

#include <vector>

#include <QString>

#include <opencv2/core.hpp>

namespace vision {

struct TemplateMatch
{
    int digit = 0;      ///< 1..7，0 表示未判定
    double score = 0.0; ///< 归一化互相关得分
};

/// 基于字形模板的简谱数字分类器。
///
/// 为什么不直接用 Tesseract：Tesseract 的 LSTM 是为文本行训练的，
/// 把 20 px 的孤立单字逐个喂进去是它最弱的使用方式，实测只有 8/29 正确。
/// 简谱数字是标准字体渲染的，模板匹配配合结构特征（洞数）能到 28~29/29，
/// 而且不需要任何外部进程或模型文件。
class TemplateClassifier
{
public:
    /// 从 Qt 资源或文件目录加载模板 PNG
    bool load(const QString& directory, QString* error = nullptr);
    bool isLoaded() const { return !m_entries.empty(); }
    int size() const { return static_cast<int>(m_entries.size()); }

    /// inkMask 必须是"墨迹=255"的单字形掩膜
    TemplateMatch classify(const cv::Mat& inkMask) const;

    /// 闭合回路数量。只统计面积占比够大的洞，避免二值化噪声制造假洞。
    static int countHoles(const cv::Mat& inkMask);

    /// 归一化到固定画布（等比缩放 + 居中 + 轻微模糊）
    static cv::Mat normalize(const cv::Mat& inkMask);

private:
    struct Entry
    {
        int digit = 0;
        int holes = 0;
        cv::Mat norm;
    };

    std::vector<Entry> m_entries;
};

} // namespace vision
