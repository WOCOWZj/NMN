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
    QApplication a(argc, argv);
    NMN mainwindow;
    mainwindow.show();
    Imgprcs imageprocessor;
    QObject::connect(&mainwindow, &NMN::imageDropped, &imageprocessor, &Imgprcs::onImageReceived);
    imageprocessor.locateElements();
    return a.exec();
}