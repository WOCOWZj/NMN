#pragma once // 预编译指令，确保头文件只被包含一次

#include <QThread>            // Qt线程库，用于多线程处理
#include <QFile>              // Qt文件操作类
#include <QIODevice>          // Qt I/O设备基类
#include <QMessageBox>        // Qt消息框类
#include <QStringList>        // Qt字符串列表类
#include <QProcess>           // Qt进程管理类，用于启动外部程序
#include <QDir>               // Qt目录操作类
#include <QElapsedTimer>      // Qt计时器类，用于测量时间间隔
#include <QFileInfo>          // Qt文件信息类
#include <opencv2/opencv.hpp> // OpenCV库，用于图像处理

// Worker类：负责执行OCR工作的对象，继承自QObject
class Worker : public QObject
{
Q_OBJECT // Qt元对象系统宏，启用信号槽机制

    private : QString rect_path = "assets/rect.png"; // 临时保存矩形区域图像的路径
    QString output_noext = "assets/rect";            // Tesseract输出文件路径（不含扩展名）
    QString output_withext = "assets/rect.txt";      // Tesseract输出文件路径（含扩展名）

public slots:
    // doWork槽函数：执行OCR处理工作
    // 参数：image - 要处理的图像，rect_list - 要识别的矩形区域列表，whitelist - 字符白名单（可选）
    void doWork(cv::Mat image, const QList<QRect> &rect_list, const QString &whitelist = "");

signals:
    // ResultReady信号：OCR处理完成后发出，传递识别结果
    void ResultReady(const QList<QString> &result);
};

// Controller类：管理Worker线程的控制器，继承自QObject
class Controller : public QObject
{
    Q_OBJECT                  // Qt元对象系统宏，启用信号槽机制
        QThread workerThread; // 工作线程对象

public:
    Controller();  // 构造函数：初始化控制器
    ~Controller(); // 析构函数：清理资源

signals:
    // Operate信号：触发OCR操作
    void Operate(cv::Mat image, const QList<QRect> &rect_list, const QString &whitelist = "");
    // ResultReady信号：OCR结果准备就绪
    void ResultReady(const QList<QString> &result);
};