#pragma once

#include <QImage>
#include <QList>
#include <QObject>
#include <QRect>

#include <opencv2/core.hpp>

#include "core/Score.h"
#include "vision/OcrEngine.h"

namespace pipeline {

/// 真正干活的类，被移到工作线程中执行。
/// 整个流程（预处理 → 切行 → 切格 → OCR → 组装 Score）都在这里完成，
/// 不再像原实现那样把耗时的切分留在 GUI 线程。
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

    /// 把一个 OCR 格子拆成 expectedCount 个字形框。
    /// 优先用连通域真实位置；数量对不上时退回等分，保证与旧行为一致。
    static QList<QRect> splitGlyphBoxes(const cv::Mat& binary,
                                        const QRect& cell,
                                        int expectedCount);

    vision::OcrEngine m_engine;
};

} // namespace pipeline
