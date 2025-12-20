#pragma once
#include <QThread>
#include <QFile>
#include <QIODevice>
#include <QMessageBox>
#include <QStringList>
#include <QProcess>
#include <QDir>
#include <QElapsedTimer>
#include <QFileInfo>

struct OCRresult
{
    int level;
    int page_num;
    int block_num;
    int par_num;
    int line_num;
    int word_num;
    int left;
    int top;
    int width;
    int height;
    double conf;
    QString text;

    OCRresult(const QStringList &list) : level(list[0].toInt()),
                                         page_num(list[1].toInt()),
                                         block_num(list[2].toInt()),
                                         par_num(list[3].toInt()),
                                         line_num(list[4].toInt()),
                                         word_num(list[5].toInt()),
                                         left(list[6].toInt()),
                                         top(list[7].toInt()),
                                         width(list[8].toInt()),
                                         height(list[9].toInt()),
                                         conf(list[10].toDouble()),
                                         text(list[11])
    {
    }
};

class Worker : public QObject
{
    Q_OBJECT

private:
    QString output_noext_format = "assets/%1";
    QString output_withext_format = "assets/%1.tsv";
    QList<OCRresult> parseTSV(const QString &filepath);

public slots:
    void doWork(const QString &imagePath, int psm, const QString &whitelist = "");

signals:
    void ResultReady(const QList<OCRresult> &data);
};

class Controller : public QObject
{
    Q_OBJECT
    QThread workerThread;

public:
    Controller();
    ~Controller();

signals:
    void Operate(const QString &imagePath, int psm, const QString &whitelist = "");
    void ResultReady(const QList<OCRresult> &data);
};
