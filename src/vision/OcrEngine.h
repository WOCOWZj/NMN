#pragma once

#include <QList>
#include <QRect>
#include <QString>

#include <opencv2/core.hpp>

namespace tesseract {
class TessBaseAPI;
}

namespace vision {

struct OcrConfig
{
    /// tessdata 目录。留空则交给环境变量 TESSDATA_PREFIX
    QString dataPath = QStringLiteral("D:/Tesseract-OCR/tessdata");
    /// 语言包名，与原 CLI 默认一致
    QString language = QStringLiteral("eng");
    /// 只允许出现这些字符，对应原 CLI 的 tessedit_char_whitelist
    QString characterWhitelist = QStringLiteral("01234567");
    /// tesseract::PageSegMode 数值，10 = PSM_SINGLE_CHAR（对应原 CLI 的 --psm 10）
    int pageSegMode = 10;
};

/// 直接链接 libtesseract 的识别器。
/// 原先每个格子都要 QProcess 启动一次 tesseract.exe，一整页 26 个格子就是 26 次进程创建；
/// 现在进程内初始化一次 TessBaseAPI，之后每个格子只做一次 SetImage + GetUTF8Text。
/// 同步执行，必须在工作线程里调用。
class OcrEngine
{
public:
    explicit OcrEngine(OcrConfig config = OcrConfig());
    ~OcrEngine();

    OcrEngine(const OcrEngine&) = delete;
    OcrEngine& operator=(const OcrEngine&) = delete;

    /// 修改配置会使已初始化的引擎失效，下次识别时重新 Init
    void setConfig(const OcrConfig& config);
    const OcrConfig& config() const { return m_config; }

    /// tessdata 是否就绪
    bool isAvailable(QString* reason = nullptr) const;

    /// 逐区域识别。返回列表与 regions 等长，失败的格子对应空串。
    QList<QString> recognize(const cv::Mat& image,
                             const QList<QRect>& regions,
                             QString* error = nullptr);

private:
    bool ensureApi(QString* error);
    void releaseApi();

    OcrConfig m_config;
    tesseract::TessBaseAPI* m_api = nullptr;
    bool m_ready = false;
};

} // namespace vision
