#include "tessthread.h"

void Worker::doWork(const QString &imagePath)
{
    QFile logFile("NMN.log");
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        QTextStream out(&logFile);
        out << "OCR start" << "\n";
        logFile.close();
    }

    QStringList args;
    QString tessPath = "D:/Tesseract-OCR/tesseract.exe";

    args = {imagePath, "output", "--psm", "6", "-c", "tessedit_char_whitelist=01234567", "tsv"};

    QProcess process;
    process.start(tessPath, args);
    process.waitForFinished();

    {
        QFile logFile("NMN.log");
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            QTextStream out(&logFile);
            out << "ret\n"
                << process.readAllStandardError() << "\n";
            logFile.close();
        }
    }

    QString result;
    if (process.exitCode() == 0)
    {
        QFile file("output.tsv");
        if (file.open(QIODevice::ReadOnly))
        {
            result = file.readAll();
            file.close();
            QFile::remove("output.tsv");

            QFile logFile("NMN.log");
            if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
            {
                QTextStream out(&logFile);
                out << "OCR success" << "\n";
                logFile.close();
            }
        }
    }

    QFile::remove(imagePath);

    emit resultReady(result);
}

Controller::Controller()
{
    Worker *worker = new Worker;
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &Controller::operate, worker, &Worker::doWork);
    connect(worker, &Worker::resultReady, this, &Controller::resultRecieve);
    workerThread.start();
}

Controller::~Controller()
{
    workerThread.quit();
    workerThread.wait();
}

void Controller::resultRecieve(const QString &result)
{
    emit resultReady(result);
}