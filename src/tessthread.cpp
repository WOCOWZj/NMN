#include "tessthread.h"

void Worker::doWork(const QString &imagePath, int psm, const QString &whitelist)
{
    QFileInfo fileinfo(imagePath);
    QString basename = fileinfo.baseName();
    QString output_noext = this->output_noext_format.arg(basename);
    QString output_withext = this->output_withext_format.arg(basename);

    QFile logFile("NMN.log");
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        QTextStream out(&logFile);
        if (!QFile::exists(imagePath))
        {
            out << imagePath << " does not exist\n";
            logFile.close();
            QMessageBox::critical(nullptr, "Error", QString("cannot open %1\n").arg(imagePath));
            return;
        }
        out << QString("OCR start on %1\n").arg(imagePath);
        logFile.close();
    }

    QStringList args;
    QString tessPath = "D:/Tesseract-OCR/tesseract.exe";

    args << imagePath << output_noext << "--psm" << QString::number(psm);
    if (whitelist != "")
        args << "-c" << "tessedit_char_whitelist=" + whitelist;
    args << "tsv";

    QElapsedTimer timer;
    timer.start();

    QProcess process;
    process.start(tessPath, args);
    process.waitForFinished();

    QString result;
    if (process.exitCode() == 0)
    {
        QFile file(output_withext);
        if (file.open(QIODevice::ReadOnly))
        {
            result = file.readAll();
            file.close();

            {
                QFile logFile("NMN.log");
                if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
                {
                    QTextStream out(&logFile);
                    out << "OCR success" << "\n";
                    out << "taking " << timer.elapsed() << "ms\n";
                    logFile.close();
                }
            }
        }
    }

    QFile::remove(imagePath);

    QList<OCRresult> data = this->parseTSV(output_withext);

    // QFile::remove(output_withext);

    emit ResultReady(data);
}

QList<OCRresult> Worker::parseTSV(const QString &filepath)
{
    QFile tsvFile(filepath);

    if (!tsvFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QFile logFile("NMN.log");
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            QTextStream out(&logFile);
            out << "parse failed : cannot open" << "\n";
            logFile.close();
        }
        QMessageBox::critical(nullptr, "Error", "cannot open tsv");
    }

    QTextStream in(&tsvFile);

    QList<OCRresult> retList;

    while (!in.atEnd())
    {
        QString line = in.readLine();
        OCRresult ocr_result(line.split('\t'));
        retList.append(ocr_result);
    }
    if (!retList.isEmpty())
        retList.removeFirst();

    tsvFile.close();

    return retList;
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