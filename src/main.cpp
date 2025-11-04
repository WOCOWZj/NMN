#include "NMN.h"
#include <QApplication>
#include <opencv2/opencv.hpp>
#pragma comment(lib, "user32.lib")

int main(int argc, char *argv[])
{
    cv::Mat image = cv::imread("image.jpg", cv::IMREAD_COLOR);

    QApplication a(argc, argv);
    NMN w;
    w.show();
    return a.exec();
}