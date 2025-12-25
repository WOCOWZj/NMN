#include "tessthread.h"

void Worker::doWork(cv::Mat image, const QList<QRect> &rect_list, const QString &whitelist)
{
    {
        QFile logFile("NMN.log");
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            QTextStream out(&logFile);
            out << "OCR started..." << "\n";
            logFile.close();
        }
    }

    QList<QString> result;

    QElapsedTimer timer;
    timer.start();

    for (const QRect &rect : rect_list)
    {
        cv::Mat rect_image = image(cv::Rect(rect.x(), rect.y(), rect.width(), rect.height())).clone();

        if (!cv::imwrite(this->rect_path.toStdString(), rect_image))
        {
            QMessageBox::critical(nullptr, "Error", "Failed to Save");
            {
                QFile logFile("NMN.log");
                if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
                {
                    QTextStream out(&logFile);
                    out << QString("cannot save rect") << "\n";
                    logFile.close();
                }
            }
        }

        QString tessPath = "D:/Tesseract-OCR/tesseract.exe";
        QStringList args;
        args << this->rect_path << this->output_noext << "--psm" << "10";
        if (whitelist != "")
            args << "-c" << "tessedit_char_whitelist=" + whitelist;

        QProcess process;
        process.start(tessPath, args);
        process.waitForFinished();

        if (process.exitCode() == 0)
        {
            QFile file(this->output_withext);
            if (file.open(QIODevice::ReadOnly))
            {
                result.append(QString(file.readAll()).remove(" ").remove("\n"));
                file.close();
            }
        }
    }

    {
        QFile logFile("NMN.log");
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            QTextStream out(&logFile);
            out << QString("OCR finished, taking %1 ms\n").arg(timer.elapsed());
        }
    }

    QFile::remove(this->rect_path);
    QFile::remove(this->output_withext);

    emit ResultReady(result);
}

Controller::Controller()
{
    Worker *worker = new Worker;
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &Controller::Operate, worker, &Worker::doWork);
    connect(worker, &Worker::ResultReady, this, &Controller::ResultReady);
    workerThread.start();
}

Controller::~Controller()
{
    workerThread.quit();
    workerThread.wait();
}