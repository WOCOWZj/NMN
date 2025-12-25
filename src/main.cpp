#include "NMN.h"
#include "imgprcs.h"
#include "highlight.h"
#include "convert.h"
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
    ImgProcessor imageprocessor;
    HighLight highlight;
    Converter converter;

    QObject::connect(&mainwindow, &NMN::ImageDropped, &imageprocessor, &ImgProcessor::onImageReceived);
    QObject::connect(&imageprocessor, &ImgProcessor::ImageProcessed, &highlight, &HighLight::onImageProcessed);
    QObject::connect(highlight.label, &HighLightLabel::ResultReady, &converter, &Converter::onResultReady);

    mainwindow.show();

    imageprocessor.onImageReceived("test.jpg");

    return a.exec();
}