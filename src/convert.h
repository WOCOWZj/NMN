#pragma once // 预编译指令，确保头文件只被包含一次

#include <QObject>     // Qt对象基类
#include <QList>       // Qt列表容器
#include <QFile>       // Qt文件操作类
#include <QPainter>    // Qt绘图类
#include <QPixmap>     // Qt像素图类
#include <QString>     // Qt字符串类
#include <QFileDialog> // Qt文件对话框类
#include <QMessageBox> // Qt消息框类

#include "noteclass.h" // 音符类定义

// Converter类：转换器，继承自QWidget
class Converter : public QWidget
{
    Q_OBJECT // Qt元对象系统宏，启用信号槽机制

        public slots :
        // onResultReady槽：处理结果就绪信号
        // 参数：image - 原始图像，tune - 调号，notes - 音符列表
        void onResultReady(const QPixmap &image, const Note &tune, const QList<Note> &notes);

private:
    int tune2int;            // 调号转换后的整数值
    QPixmap origin_image;    // 原始图像
    QList<Note> notes;       // 音符列表
    QPixmap converted_image; // 转换后的图像
    // convert方法：执行转换操作
    void convert();
    // adaptFontSize方法：自适应调整字体大小
    void adaptFontSize(QPainter &painter, const QString &text);
    // output方法：输出转换后的图像
    // 参数：parent - 父窗口
    bool output(QWidget *parent = nullptr);
};