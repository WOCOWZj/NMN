#include "NMN.h"                   // 主窗口类
#include "imgprcs.h"               // 图像处理类
#include "highlight.h"             // 高亮显示类
#include "convert.h"               // 转换类
#include <QApplication>            // Qt应用程序类
#include <QString>                 // Qt字符串类
#include <QObject>                 // Qt对象基类
#include <opencv2/opencv.hpp>      // OpenCV库
#pragma comment(lib, "user32.lib") // 链接Windows用户32库

// main函数：应用程序入口点
int main(int argc, char *argv[])
{
    // 记录应用程序启动日志
    QFile logFile("NMN.log");                                                     // 创建日志文件对象
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) // 以追加模式打开日志文件
    {
        QTextStream out(&logFile);                                                   // 创建文本流
        out << "\n-----------------------\n"                                         // 写入分隔线
            << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n"; // 写入当前时间戳
        logFile.close();                                                             // 关闭文件
    }

    QApplication a(argc, argv); // 创建Qt应用程序实例

    // 创建各个组件对象
    NMN mainwindow;              // 主窗口对象
    ImgProcessor imageprocessor; // 图像处理器对象
    HighLight highlight;         // 高亮显示对象
    Converter converter;         // 转换器对象

    // 建立信号槽连接，形成处理链
    // 当主窗口接收到图像时，通知图像处理器
    QObject::connect(&mainwindow, &NMN::ImageDropped, &imageprocessor, &ImgProcessor::onImageReceived);
    // 当图像处理器处理完成后，通知高亮显示模块
    QObject::connect(&imageprocessor, &ImgProcessor::ImageProcessed, &highlight, &HighLight::onImageProcessed);
    // 当高亮标签完成处理后，通知转换器
    QObject::connect(highlight.label, &HighLightLabel::ResultReady, &converter, &Converter::onResultReady);

    mainwindow.show(); // 显示主窗口

    // imageprocessor.onImageReceived("test.jpg"); // 注释掉的测试代码：直接加载测试图像

    return a.exec(); // 启动事件循环并返回退出码
}