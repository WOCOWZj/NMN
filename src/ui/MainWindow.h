#pragma once

#include <QImage>
#include <QMainWindow>
#include <QPointer>
#include <QString>

#include "core/Score.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

namespace pipeline {
class RecognitionPipeline;
}

class ScoreView;

/// 主窗口：只负责拖拽接收、展示进度、以及把识别流水线与结果视图接起来。
/// 取代原 NMN 类。
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    /// 打开一张图片并启动识别
    void openImage(const QString& filePath);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private slots:
    void onProgress(const QString& message);
    void onRecognitionFailed(const QString& reason);
    void onRecognitionFinished(const core::Score& score);
    void onScoreSubmitted(const core::Score& score);

private:
    void displayImage(const QImage& image);
    void exportScore(const core::Score& score);

    Ui::MainWindow* m_ui = nullptr;
    pipeline::RecognitionPipeline* m_pipeline = nullptr;

    QImage m_sourceImage;
    QString m_sourcePath;

    QPointer<ScoreView> m_scoreView;
};
