#pragma once

#include <vector>

#include <opencv2/core.hpp>

namespace vision {

/// 八度记号：简谱用数字上方/下方的小圆点表示高/低八度
enum class Octave
{
    None,
    Lower,
    Upper
};

/// 一行乐谱的纵向范围
struct RowBand
{
    int top = 0;
    int bottom = 0;
    int ink = 0;
};

/// 一个候选字形
struct GlyphBox
{
    cv::Rect box;
    int area = 0;
    Octave octave = Octave::None;
};

/// 一个孤立小部件（八度点、附点等）
struct MarkBox
{
    cv::Rect box;
    int area = 0;
};

struct Extraction
{
    std::vector<GlyphBox> glyphs;
    std::vector<MarkBox> marks;
    int rowCount = 0;
};

/// 把二值墨迹图切成候选字形。
///
/// 相比"列投影找连通段"的旧做法，这里分三步：
///   1. 水平开运算剥离减时线——它会把相邻音符连成一整坨；
///   2. 垂直投影选出真正的乐谱行，排除标题与词曲作者等文字；
///   3. 行内做连通域分析，粘连的多个音符再按列投影谷点拆开。
class GlyphExtractor
{
public:
    /// 去掉贯穿多个音符的长横线（减时线、连谱线）
    static cv::Mat suppressStaffLines(const cv::Mat& ink);

    /// 垂直投影检测候选行
    static std::vector<RowBand> detectRows(const cv::Mat& cleanedInk);

    /// 从候选行里挑出真正的乐谱行：字形足够多、墨量足够大
    static std::vector<RowBand> selectMusicRows(const cv::Mat& cleanedInk,
                                                const std::vector<RowBand>& candidates);

    /// 在单行内提取字形与小部件
    static Extraction extractRow(const cv::Mat& cleanedInk, const RowBand& band);

    /// 便捷入口：检测 → 选择 → 逐行提取 → 八度归属
    static Extraction extract(const cv::Mat& cleanedInk);

    /// 把小部件归属为最近字形的八度点（仅取正上方/正下方，排除右侧附点）
    static void associateOctaves(Extraction& extraction);
};

} // namespace vision
