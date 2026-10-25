#pragma once

#include <QList>
#include <QRect>
#include <QString>

#include <opencv2/core.hpp>

namespace vision {

struct OcrConfig
{
    QString executablePath = QStringLiteral("D:/Tesseract-OCR/tesseract.exe");
    QString scratchDirectory = QStringLiteral("assets");
    QString characterWhitelist = QStringLiteral("01234567");
    int pageSegMode = 10;
    int timeoutMs = 20000;
};

/// Tesseract 命令行封装。
/// 同步执行，必须在工作线程里调用（内部会阻塞等待 QProcess）。
class OcrEngine
{
public:
    explicit OcrEngine(OcrConfig config = OcrConfig());

    void setConfig(const OcrConfig& config) { m_config = config; }
    const OcrConfig& config() const { return m_config; }

    /// 可执行文件是否就绪；不就绪时通过 reason 返回原因
    bool isAvailable(QString* reason = nullptr) const;

    /// 逐区域识别。返回列表与 regions 等长，识别失败的格子对应空串，
    /// 以保证调用方可以按下标与原始区域一一对应。
    QList<QString> recognize(const cv::Mat& image,
                             const QList<QRect>& regions,
                             QString* error = nullptr);

private:
    QString ensureScratchDirectory() const;

    OcrConfig m_config;
};

} // namespace vision
