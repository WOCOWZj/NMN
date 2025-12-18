#pragma once
#include <QThread>
#include <QFile>
#include <QIODevice>
#include <QMessageBox>
#include <QStringList>
#include <QProcess>
#include <QDir>

class Worker : public QObject
{
    Q_OBJECT

public slots:
    void doWork(const QString &imagePath);

signals:
    void resultReady(const QString &result);
};

class Controller : public QObject
{
    Q_OBJECT
    QThread workerThread;

public:
    Controller();
    ~Controller();

public slots:
    void resultRecieve(const QString &result);

signals:
    void operate(const QString &imagePath);
    void resultReady(const QString &result);
};
