#pragma once
#include <string>
#include <QObject>
#include <opencv2/opencv.hpp>
#include <QRect>
#include <QMessageBox>


#include "tessthread.h"

class Imgprcs : public QObject
{
    Q_OBJECT
    QString pretreated_image_path = "build/assets/pretreated_image.png";

public:
    cv::Mat img;

public slots:
    void onImageReceived(const QString &filePath);

public:
    cv::Mat pretreated_image;
    void pretreat();
    void onOCRReceicved(const QString &result);

};