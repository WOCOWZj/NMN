#include "vision/OcrEngine.h"

#include "util/Logger.h"

#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QStringList>

#include <opencv2/imgcodecs.hpp>

namespace vision {

namespace {
constexpr char kScratchBase[] = "rect";
}

OcrEngine::OcrEngine(OcrConfig config)
    : m_config(std::move(config))
{
}

bool OcrEngine::isAvailable(QString* reason) const
{
    if (m_config.executablePath.trimmed().isEmpty())
    {
        if (reason)
            *reason = QStringLiteral("未配置 Tesseract 可执行文件路径");
        return false;
    }

    if (!QFileInfo::exists(m_config.executablePath))
    {
        if (reason)
            *reason = QStringLiteral("找不到 Tesseract：%1").arg(m_config.executablePath);
        return false;
    }

    return true;
}

QString OcrEngine::ensureScratchDirectory() const
{
    const QString directory = m_config.scratchDirectory;
    if (directory.isEmpty())
        return QString();

    QDir dir(directory);
    if (!dir.exists() && !QDir().mkpath(directory))
    {
        util::Logger::warn(QStringLiteral("无法创建 OCR 临时目录：%1").arg(directory));
        return QString();
    }
    return directory;
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

    const QString scratch = ensureScratchDirectory();
    if (scratch.isEmpty())
    {
        if (error)
            *error = QStringLiteral("OCR 临时目录不可用");
        return results;
    }

    const QString imagePath = QDir(scratch).filePath(QString::fromLatin1(kScratchBase) + QStringLiteral(".png"));
    const QString outputBase = QDir(scratch).filePath(QString::fromLatin1(kScratchBase));
    const QString textPath = outputBase + QStringLiteral(".txt");

    const QRect imageBounds(0, 0, image.cols, image.rows);
    QElapsedTimer timer;
    timer.start();

    int failures = 0;

    for (const QRect& region : regions)
    {
        const QRect clipped = region.intersected(imageBounds);
        if (clipped.width() <= 0 || clipped.height() <= 0)
        {
            results.append(QString());
            ++failures;
            continue;
        }

        const cv::Mat roi = image(cv::Rect(clipped.x(), clipped.y(), clipped.width(), clipped.height())).clone();
        if (!cv::imwrite(imagePath.toStdString(), roi))
        {
            results.append(QString());
            ++failures;
            continue;
        }

        QStringList arguments;
        arguments << imagePath
                  << outputBase
                  << QStringLiteral("--psm")
                  << QString::number(m_config.pageSegMode);
        if (!m_config.characterWhitelist.isEmpty())
        {
            arguments << QStringLiteral("-c")
                      << QStringLiteral("tessedit_char_whitelist=") + m_config.characterWhitelist;
        }

        QProcess process;
        process.start(m_config.executablePath, arguments);

        bool ok = process.waitForFinished(m_config.timeoutMs);
        if (!ok)
        {
            process.kill();
            process.waitForFinished(1000);
        }

        if (!ok || process.exitCode() != 0)
        {
            results.append(QString());
            ++failures;
            continue;
        }

        QFile textFile(textPath);
        if (!textFile.open(QIODevice::ReadOnly))
        {
            results.append(QString());
            ++failures;
            continue;
        }

        QString text = QString::fromUtf8(textFile.readAll());
        textFile.close();

        text.remove(QLatin1Char(' ')).remove(QLatin1Char('\n')).remove(QLatin1Char('\r'));
        results.append(text);
    }

    QFile::remove(imagePath);
    QFile::remove(textPath);

    util::Logger::info(QStringLiteral("OCR 完成：%1 个区域，失败 %2，耗时 %3 ms")
                           .arg(regions.size())
                           .arg(failures)
                           .arg(timer.elapsed()));

    if (failures == regions.size() && error)
        *error = QStringLiteral("所有区域识别均失败");

    return results;
}

} // namespace vision
