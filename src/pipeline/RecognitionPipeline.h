#pragma once

#include <QImage>
#include <QObject>
#include <QThread>

#include "core/Score.h"

namespace pipeline {

class RecognitionWorker;

/// 识别流水线的对外门面：拥有工作线程，负责把 QImage 送进去、把 Score 送出来。
/// 取代原先 ImgProcessor 在 OCR 里 `new Controller` 且从不释放的做法。
class RecognitionPipeline : public QObject
{
    Q_OBJECT

public:
    explicit RecognitionPipeline(QObject* parent = nullptr);
    ~RecognitionPipeline() override;

    bool isBusy() const { return m_busy; }

public slots:
    void start(const QImage& image);

signals:
    void progress(const QString& message);
    void failed(const QString& reason);
    void finished(const core::Score& score);

signals:
    // 内部信号：用于跨线程触发 worker
    void runRequested(const QImage& image);

private:
    void setBusy(bool busy);

    QThread m_thread;
    RecognitionWorker* m_worker = nullptr;
    bool m_busy = false;
};

} // namespace pipeline
