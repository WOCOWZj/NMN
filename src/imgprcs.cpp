#include "imgprcs.h"

void Imgprcs::onImageRecieved(const QString filepath)
{
    this->img = cv::imread(filepath.toStdString());
    cv::namedWindow("src", cv::WINDOW_AUTOSIZE);
    cv::imshow("src", img);
    return;
}