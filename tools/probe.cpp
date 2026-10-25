// 离线探针：用生产代码里的视觉组件跑识别，并对照人工标注算准确率。
// 仅供开发调试，构建目标为 EXCLUDE_FROM_ALL。
//
// 用法： nmn_probe <图片路径> <模板目录>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <vector>

#include <opencv2/opencv.hpp>

#include "vision/GlyphExtractor.h"
#include "vision/ImageNormalizer.h"
#include "vision/TemplateClassifier.h"

namespace {

const std::vector<int> kGroundTruth = {
    2, 3, 5, 6,
    1, 1, 3, 2, 1, 1, 6, 1, 6,
    5, 6, 1, 2, 3,
    2, 3, 5, 6,
    1, 1, 3, 2, 1, 1, 6};

struct Run
{
    std::vector<int> digits;
    std::vector<double> scores;
    std::vector<int> holes;
    int glyphCount = 0;
    double medianHeight = 0;
};

Run analyze(const cv::Mat& bgr, double scale, const vision::TemplateClassifier& classifier)
{
    Run run;

    cv::Mat working;
    if (scale > 1.01)
        cv::resize(bgr, working, cv::Size(), scale, scale, cv::INTER_CUBIC);
    else
        working = bgr;

    const cv::Mat ink = vision::ImageNormalizer::binarize(working);
    const cv::Mat cleaned = vision::GlyphExtractor::suppressStaffLines(ink);

    const std::vector<vision::RowBand> rows =
        vision::GlyphExtractor::selectMusicRows(cleaned, vision::GlyphExtractor::detectRows(cleaned));

    std::vector<vision::GlyphBox> glyphs;
    std::vector<int> heights;

    for (const auto& band : rows)
    {
        const vision::Extraction row = vision::GlyphExtractor::extractRow(cleaned, band);
        for (const auto& glyph : row.glyphs)
        {
            glyphs.push_back(glyph);
            heights.push_back(glyph.box.height);
        }
    }

    run.glyphCount = static_cast<int>(glyphs.size());
    if (heights.empty())
        return run;

    std::sort(heights.begin(), heights.end());
    run.medianHeight = heights[heights.size() / 2];

    for (const auto& glyph : glyphs)
    {
        const cv::Mat mask = cleaned(glyph.box);
        const vision::TemplateMatch match = classifier.classify(mask);
        run.digits.push_back(match.digit);
        run.scores.push_back(match.score);
        run.holes.push_back(vision::TemplateClassifier::countHoles(mask));
    }

    return run;
}

void report(const char* label, const Run& run)
{
    size_t matched = 0;
    const size_t compared = std::min(run.digits.size(), kGroundTruth.size());
    for (size_t i = 0; i < compared; ++i)
        if (run.digits[i] == kGroundTruth[i])
            ++matched;

    std::printf("%-22s 字 %2zu/%zu 中位高 %.1f 正确 %2zu | ", label, run.digits.size(),
                kGroundTruth.size(), run.medianHeight, matched);
    for (int value : run.digits)
        std::printf("%d", value);
    std::printf("\n%-22s 差异 ", "");
    for (size_t i = 0; i < compared; ++i)
        std::printf(run.digits[i] == kGroundTruth[i] ? "." : "X");
    std::printf("   洞 ");
    for (int hole : run.holes)
        std::printf("%d", hole);
    std::printf("   最低分 %.3f\n\n",
                run.scores.empty() ? 0.0 : *std::min_element(run.scores.begin(), run.scores.end()));
}

} // namespace

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        std::printf("usage: nmn_probe <image> <templateDir>\n");
        return 1;
    }

    const cv::Mat src = cv::imread(argv[1]);
    if (src.empty())
    {
        std::printf("无法读取图片: %s\n", argv[1]);
        return 1;
    }

    vision::TemplateClassifier classifier;
    QString error;
    if (!classifier.load(QString::fromLocal8Bit(argv[2]), &error))
    {
        std::printf("模板加载失败: %s\n", qPrintable(error));
        return 1;
    }

    std::printf("模板 %d 个\n", classifier.size());
    std::printf("ground truth (%zu): ", kGroundTruth.size());
    for (int value : kGroundTruth)
        std::printf("%d", value);
    std::printf("\n\n");

    // 第一遍估计字号
    const Run probe = analyze(src, 1.0, classifier);
    report("原生 x1.0", probe);

    // 按目标字高自动定标
    double scale = 1.0;
    if (probe.medianHeight > 0 && probe.medianHeight < 40.0)
    {
        scale = std::min(3.0, std::max(1.0, 40.0 / probe.medianHeight));
        scale = std::round(scale * 4.0) / 4.0;
    }

    if (scale > 1.01)
    {
        char label[32];
        std::snprintf(label, sizeof(label), "自动放大 x%.2f", scale);
        report(label, analyze(src, scale, classifier));
    }

    // 参考：几档固定缩放
    for (double fixed : {1.5, 2.0, 2.5})
    {
        char label[32];
        std::snprintf(label, sizeof(label), "固定 x%.1f", fixed);
        report(label, analyze(src, fixed, classifier));
    }

    return 0;
}
