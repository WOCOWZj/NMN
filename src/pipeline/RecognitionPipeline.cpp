#include "pipeline/RecognitionPipeline.h"

#include "pipeline/RecognitionWorker.h"
#include "util/Logger.h"

namespace pipeline {

RecognitionPipeline::RecognitionPipeline(QObject* parent)
    : QObject(parent)
    , m_worker(new RecognitionWorker)
{
    m_worker->moveToThread(&m_thread);

    connect(&m_thread, &QThread::finished, m_worker, &QObject::deleteLater);

    // 跨线程均为队列连接，QImage / core::Score 都已注册为元类型
    connect(this, &RecognitionPipeline::runRequested, m_worker, &RecognitionWorker::run);

    connect(m_worker, &RecognitionWorker::progress, this, [this](const QString& message) {
        emit progress(message);
    });

    connect(m_worker, &RecognitionWorker::failed, this, [this](const QString& reason) {
        setBusy(false);
        util::Logger::warn(QStringLiteral("识别失败：%1").arg(reason));
        emit failed(reason);
    });

    connect(m_worker, &RecognitionWorker::finished, this, [this](const core::Score& score) {
        setBusy(false);
        emit finished(score);
    });

    m_thread.start();
}

RecognitionPipeline::~RecognitionPipeline()
{
    m_thread.quit();
    m_thread.wait();
    // m_worker 由 QThread::finished → deleteLater 负责回收
}

void RecognitionPipeline::setBusy(bool busy)
{
    m_busy = busy;
}

void RecognitionPipeline::start(const QImage& image)
{
    if (m_busy)
    {
        util::Logger::warn(QStringLiteral("上一次识别尚未结束，忽略本次请求"));
        return;
    }

    setBusy(true);
    emit runRequested(image);
}

} // namespace pipeline
