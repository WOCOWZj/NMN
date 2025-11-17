#pragma once

#include <QObject>
#include <opencv2/opencv.hpp>

class Imgprcs : public QObject
{
    Q_OBJECT

public:
    cv::Mat img;

public slots:
    void onImageRecieved(const QString filePath);

private:
    void pretreat();
    void locateElements();
};