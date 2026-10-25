#include "vision/GlyphExtractor.h"

#include <algorithm>
#include <cmath>

#include <opencv2/imgproc.hpp>

namespace vision {

namespace {

constexpr int kMinComponentArea = 6;
constexpr int kRowGapTolerance = 3;   // 行内允许的空白行数
constexpr double kMinGlyphRatio = 0.6; // 字形高度 / 行内中位高度
constexpr double kMaxGlyphRatio = 1.5;
constexpr int kMaxMarkArea = 60;

double medianOf(std::vector<int> values)
{
    if (values.empty())
        return 0.0;
    std::sort(values.begin(), values.end());
    const size_t n = values.size();
    if (n % 2 == 1)
        return static_cast<double>(values[n / 2]);
    return 0.5 * (values[n / 2 - 1] + values[n / 2]);
}

struct Component
{
    cv::Rect box;
    int area = 0;
};

/// 细长且没有贯穿全高的实心主笔 → 视作弧线装饰而非音符。
/// 实测：数字 "1" 的最粗一列占满字高（1.00），而反复记号的左括号只有 0.84。
bool looksLikeArc(const cv::Mat& glyphMask)
{
    if (glyphMask.empty() || glyphMask.rows < 4)
        return false;

    const double aspect = static_cast<double>(glyphMask.cols) / glyphMask.rows;
    if (aspect >= 0.45)
        return false;

    int bestColumn = 0;
    for (int x = 0; x < glyphMask.cols; ++x)
        bestColumn = std::max(bestColumn, cv::countNonZero(glyphMask.col(x)));

    return static_cast<double>(bestColumn) / glyphMask.rows < 0.9;
}

/// 按列投影的谷点把粘连的多个音符拆开。
/// 只在连通域明显宽于单个音符时调用。
std::vector<cv::Rect> splitWideComponent(const cv::Mat& mask,
                                         const cv::Rect& box,
                                         double targetWidth)
{
    std::vector<cv::Rect> pieces;

    const int expected = static_cast<int>(std::round(box.width / std::max(1.0, targetWidth)));
    if (expected < 2)
    {
        pieces.push_back(box);
        return pieces;
    }

    const cv::Mat roi = mask(box);
    if (roi.empty())
    {
        pieces.push_back(box);
        return pieces;
    }

    std::vector<int> columns(static_cast<size_t>(roi.cols), 0);
    int maxCount = 0;
    for (int x = 0; x < roi.cols; ++x)
    {
        columns[static_cast<size_t>(x)] = cv::countNonZero(roi.col(x));
        maxCount = std::max(maxCount, columns[static_cast<size_t>(x)]);
    }

    if (maxCount <= 0)
    {
        pieces.push_back(box);
        return pieces;
    }

    // 谷点：局部最小且墨量低于峰值的三成
    std::vector<int> cuts;
    for (int x = 1; x + 1 < roi.cols; ++x)
    {
        const int here = columns[static_cast<size_t>(x)];
        if (here <= columns[static_cast<size_t>(x - 1)] &&
            here <= columns[static_cast<size_t>(x + 1)] &&
            here < 0.3 * maxCount)
        {
            if (cuts.empty() || x - cuts.back() >= 3)
                cuts.push_back(x);
        }
    }

    if (cuts.empty())
    {
        pieces.push_back(box);
        return pieces;
    }

    int left = 0;
    for (int cut : cuts)
    {
        if (cut - left >= 4)
        {
            pieces.push_back(cv::Rect(box.x + left, box.y, cut - left, box.height));
            left = cut;
        }
    }
    if (box.width - left >= 4)
        pieces.push_back(cv::Rect(box.x + left, box.y, box.width - left, box.height));

    if (pieces.empty())
        pieces.push_back(box);
    return pieces;
}

} // namespace

cv::Mat GlyphExtractor::suppressStaffLines(const cv::Mat& ink)
{
    if (ink.empty())
        return ink;

    cv::Mat cleaned = ink.clone();

    // 核长卡在 [15, 30]：数字最宽的横笔画（5 的上横、2 的下横）约 10~14 px，
    // 而减时线通常跨 25 px 以上，两者可以干净分开。
    // 上限要随图像分辨率一起放大：若固定钳到 30，图像放大后数字自身的
    // 横笔画会超过核长而被当成减时线误删，字形随即碎裂。
    int kernelLength = ink.cols / 40;
    kernelLength = std::max(15, std::min(60, kernelLength));

    if (kernelLength < ink.cols)
    {
        const cv::Mat kernel =
            cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernelLength, 1));
        cv::Mat lines;
        cv::morphologyEx(cleaned, lines, cv::MORPH_OPEN, kernel);
        cv::subtract(cleaned, lines, cleaned);
    }

    return cleaned;
}

std::vector<RowBand> GlyphExtractor::detectRows(const cv::Mat& cleanedInk)
{
    std::vector<RowBand> bands;
    if (cleanedInk.empty())
        return bands;

    std::vector<int> profile(static_cast<size_t>(cleanedInk.rows), 0);
    for (int y = 0; y < cleanedInk.rows; ++y)
        profile[static_cast<size_t>(y)] = cv::countNonZero(cleanedInk.row(y));

    const int threshold = std::max(3, cleanedInk.cols / 100);

    int y = 0;
    while (y < cleanedInk.rows)
    {
        if (profile[static_cast<size_t>(y)] <= threshold)
        {
            ++y;
            continue;
        }

        const int start = y;
        int gap = 0;
        int inkSum = 0;

        while (y < cleanedInk.rows)
        {
            if (profile[static_cast<size_t>(y)] > threshold)
            {
                inkSum += profile[static_cast<size_t>(y)];
                gap = 0;
                ++y;
            }
            else if (++gap > kRowGapTolerance)
            {
                break;
            }
            else
            {
                ++y;
            }
        }

        RowBand band;
        band.top = start;
        band.bottom = y - gap;
        band.ink = inkSum;
        if (band.bottom - band.top >= 6)
            bands.push_back(band);
    }

    return bands;
}

std::vector<RowBand> GlyphExtractor::selectMusicRows(const cv::Mat& cleanedInk,
                                                     const std::vector<RowBand>& candidates)
{
    std::vector<RowBand> selected;
    if (candidates.empty())
        return selected;

    int maxInk = 0;
    for (const RowBand& band : candidates)
        maxInk = std::max(maxInk, band.ink);

    for (const RowBand& band : candidates)
    {
        // 墨量太少的行基本是零散文字
        if (band.ink < 0.3 * maxInk)
            continue;

        // 行的墨量在线内分布：真正的乐谱行是连续密集的，
        // 而"方文山 词"这类文字行只有零星几笔
        const cv::Mat bandMat = cleanedInk.rowRange(band.top, band.bottom);
        std::vector<int> columnCounts(static_cast<size_t>(cleanedInk.cols), 0);
        int occupied = 0;
        for (int x = 0; x < cleanedInk.cols; ++x)
        {
            columnCounts[static_cast<size_t>(x)] = cv::countNonZero(bandMat.col(x));
            if (columnCounts[static_cast<size_t>(x)] > 0)
                ++occupied;
        }

        const double coverage = static_cast<double>(occupied) / cleanedInk.cols;
        if (coverage < 0.35)
            continue;

        selected.push_back(band);
    }

    return selected;
}

Extraction GlyphExtractor::extractRow(const cv::Mat& cleanedInk, const RowBand& band)
{
    Extraction extraction;
    if (cleanedInk.empty())
        return extraction;

    const int top = std::max(0, band.top);
    const int bottom = std::min(cleanedInk.rows, band.bottom);
    if (bottom - top < 6)
        return extraction;

    const cv::Mat bandMat = cleanedInk.rowRange(top, bottom);

    cv::Mat labels;
    cv::Mat stats;
    cv::Mat centroids;
    const int count = cv::connectedComponentsWithStats(bandMat, labels, stats, centroids, 8, CV_32S);

    std::vector<Component> components;
    std::vector<int> heights;

    for (int i = 1; i < count; ++i)
    {
        const int x = stats.at<int>(i, cv::CC_STAT_LEFT);
        const int y = stats.at<int>(i, cv::CC_STAT_TOP) + top;
        const int w = stats.at<int>(i, cv::CC_STAT_WIDTH);
        const int h = stats.at<int>(i, cv::CC_STAT_HEIGHT);
        const int area = stats.at<int>(i, cv::CC_STAT_AREA);

        if (area < kMinComponentArea || h < 3 || w < 1)
            continue;

        components.push_back({cv::Rect(x, y, w, h), area});
        if (h >= 8)
            heights.push_back(h);
    }

    if (components.empty())
        return extraction;

    const double heightMedian = medianOf(heights);
    if (heightMedian <= 0)
        return extraction;

    // 单个音符的典型宽度，用于识别粘连
    std::vector<int> widths;
    for (const Component& component : components)
    {
        const double ratio = component.box.height / heightMedian;
        if (ratio >= kMinGlyphRatio && ratio <= kMaxGlyphRatio)
            widths.push_back(component.box.width);
    }
    const double widthMedian = widths.empty() ? heightMedian * 0.7 : medianOf(widths);

    for (const Component& component : components)
    {
        const double heightRatio = component.box.height / heightMedian;
        const double aspect = component.box.width / static_cast<double>(component.box.height);

        // 小节线：极细长的竖条
        if (aspect < 0.12 && component.box.height > 20)
            continue;
        // 极扁的横条：减时线碎片
        if (aspect > 3.0)
            continue;

        if (heightRatio >= kMinGlyphRatio && heightRatio <= kMaxGlyphRatio)
        {
            // 明显偏宽说明多个音符粘连，按列投影谷点拆开
            if (component.box.width > 1.7 * widthMedian)
            {
                for (const cv::Rect& piece : splitWideComponent(bandMat, cv::Rect(component.box.x, component.box.y - top, component.box.width, component.box.height), widthMedian))
                {
                    GlyphBox glyph;
                    glyph.box = cv::Rect(piece.x, piece.y + top, piece.width, piece.height);
                    glyph.area = component.area;
                    extraction.glyphs.push_back(glyph);
                }
            }
            else
            {
                GlyphBox glyph;
                glyph.box = component.box;
                glyph.area = component.area;
                extraction.glyphs.push_back(glyph);
            }
        }
        else if (heightRatio < kMinGlyphRatio && component.area <= kMaxMarkArea && aspect >= 0.3 &&
                 aspect <= 3.0)
        {
            MarkBox mark;
            mark.box = component.box;
            mark.area = component.area;
            extraction.marks.push_back(mark);
        }
    }

    // 滤掉"细长且没有贯穿全高主笔"的弧线碎片（如反复记号的左括号），
    // 再滤掉明显偏离本行基线的字形（如标注在上方的三连音数字）。
    std::vector<GlyphBox> filtered;
    filtered.reserve(extraction.glyphs.size());
    for (const GlyphBox& glyph : extraction.glyphs)
    {
        if (looksLikeArc(cleanedInk(glyph.box)))
            continue;
        filtered.push_back(glyph);
    }

    if (!filtered.empty())
    {
        std::vector<int> centers;
        centers.reserve(filtered.size());
        for (const GlyphBox& glyph : filtered)
            centers.push_back(glyph.box.y + glyph.box.height / 2);
        std::sort(centers.begin(), centers.end());
        const int medianCenter = centers[centers.size() / 2];

        std::vector<GlyphBox> onBaseline;
        onBaseline.reserve(filtered.size());
        for (const GlyphBox& glyph : filtered)
        {
            const int center = glyph.box.y + glyph.box.height / 2;
            if (std::abs(center - medianCenter) <= heightMedian * 0.4)
                onBaseline.push_back(glyph);
        }

        if (!onBaseline.empty())
            filtered = onBaseline;
    }

    extraction.glyphs = filtered;

    std::sort(extraction.glyphs.begin(), extraction.glyphs.end(),
              [](const GlyphBox& a, const GlyphBox& b) { return a.box.x < b.box.x; });

    return extraction;
}

Extraction GlyphExtractor::extract(const cv::Mat& cleanedInk)
{
    Extraction merged;
    if (cleanedInk.empty())
        return merged;

    const std::vector<RowBand> candidates = detectRows(cleanedInk);
    const std::vector<RowBand> rows = selectMusicRows(cleanedInk, candidates);

    merged.rowCount = static_cast<int>(rows.size());

    for (const RowBand& band : rows)
    {
        Extraction row = extractRow(cleanedInk, band);
        for (const GlyphBox& glyph : row.glyphs)
            merged.glyphs.push_back(glyph);
        for (const MarkBox& mark : row.marks)
            merged.marks.push_back(mark);
    }

    associateOctaves(merged);
    return merged;
}

void GlyphExtractor::associateOctaves(Extraction& extraction)
{
    for (const MarkBox& mark : extraction.marks)
    {
        const double markCenterX = mark.box.x + mark.box.width / 2.0;

        GlyphBox* best = nullptr;
        double bestDistance = 1e9;

        for (GlyphBox& glyph : extraction.glyphs)
        {
            const double glyphCenterX = glyph.box.x + glyph.box.width / 2.0;
            const double dx = std::abs(markCenterX - glyphCenterX);

            // 八度点必须落在字形正上方或正下方，容忍一定横向偏移
            if (dx > glyph.box.width * 0.45)
                continue;

            const bool above = mark.box.y + mark.box.height <= glyph.box.y + 2;
            const bool below = mark.box.y >= glyph.box.y + glyph.box.height - 2;

            // 右侧附点既不在上方也不在下方，天然被排除
            if (!above && !below)
                continue;

            if (dx < bestDistance)
            {
                bestDistance = dx;
                best = &glyph;
            }
        }

        if (!best)
            continue;

        best->octave = (mark.box.y + mark.box.height <= best->box.y + 2) ? Octave::Upper
                                                                        : Octave::Lower;
    }
}

} // namespace vision
