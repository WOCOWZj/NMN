/********************************************************************************
** Form generated from reading UI file 'NMN.ui'
**
** Created by: Qt User Interface Compiler version 6.9.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#pragma once // 预编译指令，确保头文件只被包含一次

#include <QtCore/QVariant>        // Qt变体类型
#include <QtWidgets/QApplication> // Qt应用程序类
#include <QtWidgets/QMainWindow>  // Qt主窗口类
#include <QtWidgets/QMenuBar>     // Qt菜单栏类
#include <QtWidgets/QStatusBar>   // Qt状态栏类
#include <QtWidgets/QWidget>      // Qt基础窗口部件类
#include <QtWidgets/QLabel>       // Qt标签控件类
#include <QtWidgets/QVBoxLayout>  // Qt垂直布局类
#include <QImage>                 // Qt图像类
#include <QScrollArea>            // Qt滚动区域类
#include <QSizePolicy>            // Qt大小策略类

QT_BEGIN_NAMESPACE // Qt命名空间开始

    // Ui_NMN类：NMN界面的UI定义
    class Ui_NMN
{
public:
    QWidget *centralwidget;  // 中央窗口部件
    QMenuBar *menubar;       // 菜单栏
    QStatusBar *statusbar;   // 状态栏
    QLabel *imageLabel;      // 图像标签，用于显示图像
    QLabel *infoLabel;       // 信息标签，用于显示文件信息
    QVBoxLayout *layout;     // 垂直布局管理器
    QScrollArea *scrollarea; // 滚动区域，包含图像标签

    // setupUi方法：设置UI界面
    // 参数：NMN - 主窗口指针
    void setupUi(QMainWindow *NMN)
    {
        // 创建中央窗口部件
        centralwidget = new QWidget(NMN);
        centralwidget->setObjectName("centralwidget"); // 设置对象名称
        NMN->setCentralWidget(centralwidget);          // 设置为主窗口的中央部件

        // 创建菜单栏
        menubar = new QMenuBar(NMN);
        menubar->setObjectName("menubar"); // 设置对象名称
        NMN->setMenuBar(menubar);          // 设置为主窗口的菜单栏

        // 创建状态栏
        statusbar = new QStatusBar(NMN);
        statusbar->setObjectName("statusbar"); // 设置对象名称
        NMN->setStatusBar(statusbar);          // 设置为主窗口的状态栏

        // 创建垂直布局
        layout = new QVBoxLayout(centralwidget);

        // 创建图像标签，显示拖拽提示文本
        imageLabel = new QLabel("拖拽图片到此处"); // 设置初始提示文本
        imageLabel->setAlignment(Qt::AlignCenter); // 设置文本居中对齐
        // 设置样式表：虚线边框、内边距
        imageLabel->setStyleSheet("QLabel { border: 2px dashed #ccc; padding: 20px; }");
        imageLabel->setContentsMargins(20, 20, 20, 20); // 设置内容边距

        // 创建滚动区域，包含图像标签
        scrollarea = new QScrollArea(NMN);
        scrollarea->setWidget(imageLabel);    // 设置图像标签为滚动区域的内容
        scrollarea->setWidgetResizable(true); // 设置内容可调整大小
        // 设置滚动条策略：始终隐藏垂直和水平滚动条
        scrollarea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollarea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        // 创建信息标签，显示当前文件信息
        infoLabel = new QLabel("未选择图片");     // 初始显示"未选择图片"
        infoLabel->setAlignment(Qt::AlignCenter); // 设置文本居中对齐

        // 将滚动区域和信息标签添加到布局中
        layout->addWidget(scrollarea); // 添加滚动区域
        layout->addWidget(infoLabel);  // 添加信息标签

        // 再次设置中央窗口部件（确保布局正确应用）
        NMN->setCentralWidget(centralwidget);
        NMN->setWindowTitle("NMN"); // 设置窗口标题
        NMN->resize(500, 400);      // 设置窗口初始大小

        QMetaObject::connectSlotsByName(NMN); // 连接槽函数（如果有的话）
    } // setupUi
};

namespace Ui // Ui命名空间
{
    // NMN类：继承自Ui_NMN，提供类型安全的界面访问
    class NMN : public Ui_NMN
    {
    };
} // namespace Ui

QT_END_NAMESPACE // Qt命名空间结束