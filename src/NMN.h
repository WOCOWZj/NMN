#pragma once // 预编译指令，确保头文件只被包含一次

#include <QMainWindow>        // Qt主窗口基类
#include <QLabel>             // Qt标签控件
#include <opencv2/opencv.hpp> // OpenCV库，用于图像处理
#include <QMimeData>          // Qt MIME数据类，用于拖拽操作
#include <QDragEnterEvent>    // Qt拖拽进入事件
#include <QFileInfo>          // Qt文件信息类
#include <QMessageBox>        // Qt消息框类
#include <QPixmap>            // Qt像素图类
#include <QString>            // Qt字符串类
#include <qminmax.h>          // Qt最小最大值宏定义
#include <QPainter>           // Qt绘图类
#include <QList>              // Qt列表容器
#include <QRect>              // Qt矩形类

#include "NMN_autogen/include/ui_NMN.h" // UI界面头文件（由Qt Designer生成）
#include "noteclass.h"                  // 音符类定义

// NMN类：应用程序主窗口，继承自QMainWindow
class NMN : public QMainWindow
{
Q_OBJECT // Qt元对象系统宏，启用信号槽机制

    public :
    // 构造函数：创建主窗口实例
    explicit NMN(QWidget *parent = nullptr);
    // 析构函数：清理资源
    ~NMN();

private:
    QPixmap image;     // 存储当前加载的图像
    QList<Note> rects; // 存储识别到的音符列表

protected:
    // 重写拖拽进入事件处理函数：处理文件拖拽进入窗口
    void dragEnterEvent(QDragEnterEvent *event) override;
    // 重写拖拽释放事件处理函数：处理文件拖拽到窗口
    void dropEvent(QDropEvent *event) override;

private:
    Ui_NMN *ui; // UI界面指针

signals:
    // ImageDropped信号：当图像文件被拖拽到窗口时发出
    void ImageDropped(const QString &filePath);
};