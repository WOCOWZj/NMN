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
#include <opencv2/opencv.hpp>

class Worker : public QObject
{
    Q_OBJECT

private:
    QString rect_path = "assets/rect.png";
    QString output_noext = "assets/rect";
    QString output_withext = "assets/rect.txt";

public slots:
    void doWork(cv::Mat image, const QList<QRect> &rect_list, const QString &whitelist = "");

signals:
    void ResultReady(const QList<QString> &result);
};

class Controller : public QObject
{
    Q_OBJECT
    QThread workerThread;

public:
    Controller();
    ~Controller();

signals:
    void Operate(cv::Mat image, const QList<QRect> &rect_list, const QString &whitelist = "");
    void ResultReady(const QList<QString> &result);
};
