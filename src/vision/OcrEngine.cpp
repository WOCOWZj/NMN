#include "vision/OcrEngine.h"

#include "util/Logger.h"

#include <QDir>
#include <QElapsedTimer>

#include <tesseract/baseapi.h>

#include <utility>

namespace vision {

OcrEngine::OcrEngine(OcrConfig config)
    : m_config(std::move(config))
{
}

OcrEngine::~OcrEngine()
{
    releaseApi();
}

void OcrEngine::setConfig(const OcrConfig& config)
{
    m_config = config;
    // 配置变更后必须重新 Init，否则语言/白名单不会生效
    releaseApi();
}

void OcrEngine::releaseApi()
{
    if (!m_api)
        return;

    m_api->End();
    delete m_api;
    m_api = nullptr;
    m_ready = false;
}

bool OcrEngine::isAvailable(QString* reason) const
{
    if (m_config.dataPath.trimmed().isEmpty())
        return true; // 交给 TESSDATA_PREFIX

    if (!QDir(m_config.dataPath).exists())
    {
        if (reason)
            *reason = QStringLiteral("找不到 tessdata 目录：%1").arg(m_config.dataPath);
        return false;
    }

    return true;
}

bool OcrEngine::ensureApi(QString* error)
{
    if (m_api && m_ready)
        return true;

    releaseApi();

    m_api = new tesseract::TessBaseAPI();

    const QByteArray language = m_config.language.isEmpty()
                                    ? QByteArrayLiteral("eng")
                                    : m_config.language.toUtf8();
    const QByteArray dataPath = m_config.dataPath.toUtf8();

    const int result = m_api->Init(m_config.dataPath.trimmed().isEmpty() ? nullptr : dataPath.constData(),
                                   language.constData(),
                                   tesseract::OEM_DEFAULT);

    if (result != 0)
    {
        if (error)
            *error = QStringLiteral("Tesseract 初始化失败（返回 %1），请检查 tessdata 路径：%2")
                         .arg(result)
                         .arg(m_config.dataPath);
        releaseApi();
        return false;
    }

    m_api->SetPageSegMode(static_cast<tesseract::PageSegMode>(m_config.pageSegMode));

    if (!m_config.characterWhitelist.isEmpty())
    {
        const QByteArray whitelist = m_config.characterWhitelist.toUtf8();
        m_api->SetVariable("tessedit_char_whitelist", whitelist.constData());
    }

    m_ready = true;
    util::Logger::info(QStringLiteral("Tesseract 库已初始化：语言=%1，tessdata=%2")
                           .arg(QString::fromUtf8(language))
                           .arg(m_config.dataPath));
    return true;
}

QList<QString> OcrEngine::recognize(const cv::Mat& image,
                                    const QList<QRect>& regions,
                                    QString* error)
{
    QList<QString> results;
    if (image.empty() || regions.isEmpty())
        return results;

    QString reason;
    if (!isAvailable(&reason))
    {
        if (error)
            *error = reason;
        return results;
    }

    if (!ensureApi(&reason))
    {
        if (error)
            *error = reason;
        return results;
    }

    const QRect bounds(0, 0, image.cols, image.rows);
    QElapsedTimer timer;
    timer.start();

    int failures = 0;

    for (const QRect& region : regions)
    {
        const QRect clipped = region.intersected(bounds);
        if (clipped.width() <= 0 || clipped.height() <= 0)
        {
            results.append(QString());
            ++failures;
            continue;
        }

        // 子矩阵通常不连续，SetImage 用 bytes_per_line 描述行跨度，无需深拷贝
        const cv::Mat roi = image(cv::Rect(clipped.x(), clipped.y(), clipped.width(), clipped.height()));

        m_api->SetImage(roi.data,
                        roi.cols,
                        roi.rows,
                        roi.channels(),
                        static_cast<int>(roi.step));

        char* raw = m_api->GetUTF8Text();
        if (!raw)
        {
            results.append(QString());
            ++failures;
            continue;
        }

        QString text = QString::fromUtf8(raw);
        delete[] raw;

        text.remove(QLatin1Char(' ')).remove(QLatin1Char('\n')).remove(QLatin1Char('\r'));
        results.append(text);
    }

    util::Logger::info(QStringLiteral("OCR 完成：%1 个区域，失败 %2，耗时 %3 ms")
                           .arg(regions.size())
                           .arg(failures)
                           .arg(timer.elapsed()));

    if (failures == regions.size() && error)
        *error = QStringLiteral("所有区域识别均失败");

    return results;
}

} // namespace vision
