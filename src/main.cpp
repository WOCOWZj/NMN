#include "NMN.h"
#include "imgprcs.h"
#include "marker.h"
#include <QApplication>
#include <QString>
#include <QObject>
#include <opencv2/opencv.hpp>
#pragma comment(lib, "user32.lib")

int main(int argc, char *argv[])
{
    QFile logFile("NMN.log");
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        QTextStream out(&logFile);
        out << "\n-----------------------\n"
            << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
        logFile.close();
    }

    QApplication a(argc, argv);
    NMN mainwindow;
    mainwindow.show();
    
    imgProcessor imageprocessor;
    QObject::connect(&mainwindow, &NMN::ImageDropped, &imageprocessor, &imgProcessor::onImageReceived);

    imageprocessor.onImageReceived("test.jpg");

    return a.exec();
}