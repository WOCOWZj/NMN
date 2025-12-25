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
#include <QPixmap>

#include "tessthread.h"
#include "noteclass.h"

struct lineH
{
    int top;
    int height;
};

class ImgProcessor : public QObject
{
    Q_OBJECT

private:
    cv::Mat origin_img;
    QString origin_img_path;
    int rows, cols;
    cv::Mat pretreated_image;
    QList<lineH> lineh_list;
    QList<QRect> rect_list;
    QList<Note> note_list;

public slots:
    void onImageReceived(const QString &filePath);

public:
    void pretreat();
    void hsegment();
    void vsegment();
    void ocr();
    void onResultReceived(const QList<QString> &result);

signals:
    void ImageProcessed(const QPixmap &pixmap, const QList<Note> &notes, QWidget *parent = nullptr);
};