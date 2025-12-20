#pragma once
#include <string>
#include <QObject>
#include <opencv2/opencv.hpp>
#include <QRect>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QStringList>
#include <qminmax.h>

#include "tessthread.h"

struct lineH
{
    int top;
    int height;
};

class imgProcessor : public QObject
{
    Q_OBJECT

private:
    QString pretreated_image_path = "assets/pretreated_image.png";
    QString hseg_image_path_format = "assets/hseg_%1.png";
    cv::Mat origin_img;
    cv::Mat pretreated_image;

public slots:
    void onImageReceived(const QString &filePath);

public:
    void pretreat();
    void hsegment();
};