#include "ui/MainWindow.h"

#include "pipeline/RecognitionPipeline.h"
#include "pipeline/ScoreRenderer.h"
#include "ui/ScoreView.h"
#include "ui_MainWindow.h"
#include "util/Logger.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QMimeData>
#include <QPixmap>
#include <QStatusBar>
#include <QUrl>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
    , m_pipeline(new pipeline::RecognitionPipeline(this))
{
    m_ui->setupUi(this);
    setAcceptDrops(true);

    // 原先散落在 dropEvent 里的布局设置，统一收进构造函数
    m_ui->scrollarea->setMinimumWidth(800);
    m_ui->imageLabel->setMinimumWidth(800);
    m_ui->scrollarea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_ui->scrollarea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    connect(m_pipeline, &pipeline::RecognitionPipeline::progress,
            this, &MainWindow::onProgress);
    connect(m_pipeline, &pipeline::RecognitionPipeline::failed,
            this, &MainWindow::onRecognitionFailed);
    connect(m_pipeline, &pipeline::RecognitionPipeline::finished,
            this, &MainWindow::onRecognitionFinished);
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    const QMimeData* mime = event->mimeData();
    if (mime->hasUrls() && mime->urls().size() == 1)
    {
        const QString path = mime->urls().at(0).toLocalFile();
        if (!path.isEmpty() && QFileInfo(path).isFile())
        {
            event->acceptProposedAction();
            return;
        }
    }
    event->ignore();
}

void MainWindow::dropEvent(QDropEvent* event)
{
    const QList<QUrl> urls = event->mimeData()->urls();
    if (urls.size() != 1)
    {
        event->ignore();
        return;
    }

    openImage(urls.at(0).toLocalFile());
    event->accept();
}

void MainWindow::openImage(const QString& filePath)
{
    // 整条链路只在这里读一次文件
    const QImage image(filePath);
    if (image.isNull())
    {
        QMessageBox::warning(this,
                             QStringLiteral("错误"),
                             QStringLiteral("无法读取图片：\n%1").arg(filePath));
        return;
    }

    m_sourceImage = image;
    m_sourcePath = filePath;

    displayImage(image);
    m_ui->infoLabel->setText(filePath);

    m_pipeline->start(image);
}

void MainWindow::displayImage(const QImage& image)
{
    const int targetWidth = qMax(200, m_ui->imageLabel->width() - 50);
    m_ui->imageLabel->setPixmap(
        QPixmap::fromImage(image.scaledToWidth(targetWidth, Qt::SmoothTransformation)));
}

void MainWindow::onProgress(const QString& message)
{
    statusBar()->showMessage(message);
}

void MainWindow::onRecognitionFailed(const QString& reason)
{
    statusBar()->showMessage(QStringLiteral("识别失败"));
    QMessageBox::warning(this, QStringLiteral("识别失败"), reason);
}

void MainWindow::onRecognitionFinished(const core::Score& score)
{
    if (!m_scoreView)
    {
        m_scoreView = new ScoreView(nullptr);
        m_scoreView->setAttribute(Qt::WA_DeleteOnClose);
        connect(m_scoreView, &ScoreView::submitted, this, &MainWindow::onScoreSubmitted);
    }

    m_scoreView->load(m_sourceImage, score);
    m_scoreView->show();
    m_scoreView->raise();
    m_scoreView->activateWindow();

    statusBar()->showMessage(QStringLiteral("识别完成：%1 个音符。左键改、右键删、Shift 拖拽新增、回车提交")
                                 .arg(score.size()));
}

void MainWindow::onScoreSubmitted(const core::Score& score)
{
    exportScore(score);
}

void MainWindow::exportScore(const core::Score& score)
{
    core::Score working = score;
    working.transposeTo(working.tonic());

    const QImage rendered = pipeline::ScoreRenderer::render(m_sourceImage, working);
    if (rendered.isNull())
    {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("无法保存空图像。"));
        return;
    }

    const QString fileName = QFileDialog::getSaveFileName(
        this,
        QStringLiteral("保存图像"),
        QString(),
        QStringLiteral("PNG 图像 (*.png);;JPEG 图像 (*.jpg *.jpeg);;BMP 图像 (*.bmp)"));

    if (fileName.isEmpty())
        return;

    if (!rendered.save(fileName))
    {
        QMessageBox::critical(this, QStringLiteral("错误"), QStringLiteral("保存失败"));
        return;
    }

    util::Logger::info(QStringLiteral("已保存：%1").arg(fileName));
    QMessageBox::information(this, QStringLiteral("成功"), QStringLiteral("已保存"));
}
