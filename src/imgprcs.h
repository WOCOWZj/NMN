#pragma once
#include <string>
#include <QObject>
#include <opencv2/opencv.hpp>
#include <QRect>
#include <QMessageBox>
#include <opencv2/text.hpp>

class Imgprcs : public QObject
{
    Q_OBJECT

public:
    cv::Mat img;

public slots:
    void onImageReceived(const QString filePath);

public:
    cv::Mat pretreated_image;
    void pretreat();
    void locateElements();

};