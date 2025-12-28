#pragma once // 预编译指令，确保头文件只被包含一次

#include <string>             // C++标准字符串类
#include <QObject>            // Qt对象基类
#include <opencv2/opencv.hpp> // OpenCV库，用于图像处理
#include <QRect>              // Qt矩形类
#include <QMessageBox>        // Qt消息框类
#include <QFile>              // Qt文件操作类
#include <QTextStream>        // Qt文本流类
#include <QList>              // Qt列表容器
#include <QStringList>        // Qt字符串列表
#include <qminmax.h>          // Qt最小最大值宏定义
#include <QPixmap>            // Qt像素图类

#include "tessthread.h" // OCR线程处理类
#include "noteclass.h"  // 音符类定义

// lineH结构体：表示水平线段信息
struct lineH
{
    int top;    // 线段顶部位置
    int height; // 线段高度
};

// ImgProcessor类：图像处理器，继承自QObject
class ImgProcessor : public QObject
{
Q_OBJECT // Qt元对象系统宏，启用信号槽机制

    private : cv::Mat origin_img; // 原始图像
    QString origin_img_path;      // 原始图像路径
    int rows, cols;               // 图像行数和列数
    cv::Mat pretreated_image;     // 预处理后的图像
    QList<lineH> lineh_list;      // 水平线段列表
    QList<QRect> rect_list;       // 矩形区域列表
    QList<Note> note_list;        // 音符列表

public slots:
    // onImageReceived槽：接收图像并开始处理
    // 参数：filePath - 图像文件路径
    void onImageReceived(const QString &filePath);

public:
    // pretreat方法：图像预处理
    void pretreat();
    // hsegment方法：水平分割
    void hsegment();
    // vsegment方法：垂直分割
    void vsegment();
    // ocr方法：光学字符识别
    void ocr();
    // onResultReceived方法：接收OCR结果
    // 参数：result - OCR识别结果列表
    void onResultReceived(const QList<QString> &result);

signals:
    // ImageProcessed信号：图像处理完成
    // 参数：pixmap - 处理后的图像，notes - 识别到的音符列表，parent - 父窗口
    void ImageProcessed(const QPixmap &pixmap, const QList<Note> &notes, QWidget *parent = nullptr);
};