/********************************************************************************
** Form generated from reading UI file 'NMN.ui'
**
** Created by: Qt User Interface Compiler version 6.9.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#pragma once

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QImage>
#include <QScrollArea>

QT_BEGIN_NAMESPACE

class Ui_NMN
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;
    QLabel *imageLabel;
    QLabel *infoLabel;
    QVBoxLayout *layout;
    QScrollArea *scrollarea;

    void setupUi(QMainWindow *NMN)
    {
        centralwidget = new QWidget(NMN);
        centralwidget->setObjectName("centralwidget");
        NMN->setCentralWidget(centralwidget);
        menubar = new QMenuBar(NMN);
        menubar->setObjectName("menubar");
        NMN->setMenuBar(menubar);
        statusbar = new QStatusBar(NMN);
        statusbar->setObjectName("statusbar");
        NMN->setStatusBar(statusbar);

        layout = new QVBoxLayout(centralwidget);

        imageLabel = new QLabel("拖拽图片到此处");
        imageLabel->setAlignment(Qt::AlignCenter);
        imageLabel->setStyleSheet("QLabel { border: 2px dashed #ccc; padding: 20px; }");
        imageLabel->setMinimumSize(400, 300);

        scrollarea = new QScrollArea(NMN);
        scrollarea->setWidget(imageLabel);
        scrollarea->setWidgetResizable(true);
        scrollarea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollarea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        infoLabel = new QLabel("未选择图片");
        infoLabel->setAlignment(Qt::AlignCenter);

        layout->addWidget(scrollarea);
        layout->addWidget(infoLabel);

        NMN->setCentralWidget(centralwidget);
        NMN->setWindowTitle("NMN");
        NMN->resize(500, 400);

        QMetaObject::connectSlotsByName(NMN);
    } // setupUi
};

namespace Ui
{
    class NMN : public Ui_NMN
    {
    };
} // namespace Ui

QT_END_NAMESPACE