#include "vision/TemplateClassifier.h"

#include <QDir>
#include <QFile>

#include <algorithm>
#include <cmath>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace vision {

namespace {

constexpr int kCanvas = 32;
constexpr int kInner = 24;
constexpr double kBlurSigma = 1.0;
constexpr double kHoleTieBreakMargin = 0.08; ///< 视为平手的得分差
constexpr double kMinHoleAreaRatio = 0.05;

double similarity(const cv::Mat& a, const cv::Mat& b)
{
    cv::Scalar meanA, stdA, meanB, stdB;
    cv::meanStdDev(a, meanA, stdA);
    cv::meanStdDev(b, meanB, stdB);
    if (stdA[0] < 1e-6 || stdB[0] < 1e-6)
        return 0.0;

    const cv::Mat da = a - meanA[0];
    const cv::Mat db = b - meanB[0];
    const double dot = da.dot(db);
    const double denominator = std::sqrt(da.dot(da) * db.dot(db));
    return (denominator < 1e-9) ? 0.0 : dot / denominator;
}

} // namespace

bool TemplateClassifier::load(const QString& directory, QString* error)
{
    m_entries.clear();

    const QDir dir(directory);
    if (!dir.exists())
    {
        if (error)
            *error = QStringLiteral("模板目录不存在：%1").arg(directory);
        return false;
    }

    const QStringList files = dir.entryList({QStringLiteral("*.png")}, QDir::Files, QDir::Name);

    for (const QString& name : files)
    {
        const QFileInfo info(name);
        const QString stem = info.completeBaseName();
        if (stem.isEmpty())
            continue;

        const QChar last = stem.at(stem.size() - 1);
        if (!last.isDigit())
            continue;

        const int digit = last.digitValue();
        if (digit < 1 || digit > 7)
            continue;

        QFile file(dir.filePath(name));
        if (!file.open(QIODevice::ReadOnly))
            continue;

        const QByteArray data = file.readAll();
        file.close();
        if (data.isEmpty())
            continue;

        const cv::Mat buffer(1, static_cast<int>(data.size()), CV_8UC1,
                             const_cast<char*>(data.constData()));
        const cv::Mat gray = cv::imdecode(buffer, cv::IMREAD_GRAYSCALE);
        if (gray.empty())
            continue;

        // 模板是黑字白底，转成"墨迹=255"的掩膜以统一约定
        cv::Mat mask;
        cv::threshold(gray, mask, 128, 255, cv::THRESH_BINARY_INV);

        Entry entry;
        entry.digit = digit;
        entry.holes = countHoles(mask);
        entry.norm = normalize(mask);
        m_entries.push_back(entry);
    }

    if (m_entries.empty())
    {
        if (error)
            *error = QStringLiteral("模板目录为空或全部加载失败：%1").arg(directory);
        return false;
    }

    return true;
}

cv::Mat TemplateClassifier::normalize(const cv::Mat& inkMask)
{
    cv::Mat canvas = cv::Mat::zeros(kCanvas, kCanvas, CV_32FC1);

    cv::Mat points;
    cv::findNonZero(inkMask, points);
    if (points.empty())
        return canvas;

    const cv::Rect bounds = cv::boundingRect(points);
    cv::Mat cropped;
    inkMask(bounds).convertTo(cropped, CV_32F);

    const double scale = static_cast<double>(kInner) / std::max(cropped.cols, cropped.rows);
    cv::resize(cropped, cropped, cv::Size(), scale, scale,
               scale < 1.0 ? cv::INTER_AREA : cv::INTER_LINEAR);

    // 轻微模糊：抹平字体粗细与抗锯齿差异，只留下结构
    cv::GaussianBlur(cropped, cropped, cv::Size(5, 5), kBlurSigma);

    const int left = (kCanvas - cropped.cols) / 2;
    const int top = (kCanvas - cropped.rows) / 2;
    if (left >= 0 && top >= 0 && left + cropped.cols <= kCanvas && top + cropped.rows <= kCanvas)
        cropped.copyTo(canvas(cv::Rect(left, top, cropped.cols, cropped.rows)));

    return canvas;
}

int TemplateClassifier::countHoles(const cv::Mat& inkMask)
{
    if (inkMask.empty() || inkMask.rows < 3 || inkMask.cols < 3)
        return 0;

    const int totalInk = cv::countNonZero(inkMask);
    if (totalInk <= 0)
        return 0;

    // 洞面积太小说明是二值化噪点，直接忽略
    const int minHoleArea = std::max(3, static_cast<int>(std::lround(totalInk * kMinHoleAreaRatio)));

    cv::Mat background;
    cv::bitwise_not(inkMask, background);

    cv::Mat labels;
    cv::Mat stats;
    cv::Mat centroids;
    const int count =
        cv::connectedComponentsWithStats(background, labels, stats, centroids, 4, CV_32S);

    int holes = 0;
    for (int i = 1; i < count; ++i)
    {
        const int x = stats.at<int>(i, cv::CC_STAT_LEFT);
        const int y = stats.at<int>(i, cv::CC_STAT_TOP);
        const int w = stats.at<int>(i, cv::CC_STAT_WIDTH);
        const int h = stats.at<int>(i, cv::CC_STAT_HEIGHT);
        const int area = stats.at<int>(i, cv::CC_STAT_AREA);

        const bool inside = (x > 0) && (y > 0) && (x + w < inkMask.cols) && (y + h < inkMask.rows);
        if (!inside)
            continue;

        if (area < minHoleArea)
            continue;

        ++holes;
    }

    return holes;
}

TemplateMatch TemplateClassifier::classify(const cv::Mat& inkMask) const
{
    TemplateMatch best;
    if (m_entries.empty() || inkMask.empty())
        return best;

    const cv::Mat norm = normalize(inkMask);
    const int holes = countHoles(inkMask);

    // 先按纯像素相似度排序
    std::vector<double> scores;
    scores.reserve(m_entries.size());
    for (const Entry& entry : m_entries)
        scores.push_back(similarity(norm, entry.norm));

    const size_t bestIndex =
        static_cast<size_t>(std::max_element(scores.begin(), scores.end()) - scores.begin());

    // 只在接近平手时才用洞数做裁决。
    // 洞数能稳定区分 5 与 6，但粗体在小字号下常把 5 的碗状部分粘连成假洞，
    // 因此不能让一个不可靠的结构特征否决明显的像素证据。
    const double margin = scores[bestIndex] - kHoleTieBreakMargin;

    size_t chosen = bestIndex;
    for (size_t i = 0; i < m_entries.size(); ++i)
    {
        if (scores[i] < margin)
            continue;

        if (m_entries[i].holes == holes)
        {
            // 同分区间内优先洞数一致，并取得分更高者
            if (m_entries[chosen].holes != holes || scores[i] > scores[chosen])
                chosen = i;
        }
    }

    best.digit = m_entries[chosen].digit;
    best.score = scores[chosen];
    return best;
}

} // namespace vision
