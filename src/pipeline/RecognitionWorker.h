#pragma once

#include <QImage>
#include <QObject>

#include "core/Score.h"
#include "vision/GlyphClassifier.h"
#include "vision/TemplateClassifier.h"

namespace pipeline {

/// 识别流水线的执行体，运行在工作线程中。
///
/// 流程：
///   1. 第一遍在原始分辨率上估计字号；
///   2. 字号过小时整图放大（让字形达到约 40 px），再走一遍；
///   3. 二值化 → 剥离减时线 → 垂直投影选乐谱行；
///   4. 行内连通域 → 粘连拆分 → 基线与弧线过滤；
///   5. 字形模板匹配（洞数软约束），低置信度时用 Tesseract 兜底。
class RecognitionWorker : public QObject
{
    Q_OBJECT

public:
    explicit RecognitionWorker(QObject* parent = nullptr);

public slots:
    void run(const QImage& image);

signals:
    void progress(const QString& message);
    void failed(const QString& reason);
    void finished(const core::Score& score);

private:
    static cv::Mat toBgrMat(const QImage& image);

    vision::TemplateClassifier m_templates;
    vision::GlyphClassifier m_ocrFallback;
    bool m_templatesLoaded = false;
};

} // namespace pipeline
