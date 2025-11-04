/********************************************************************************
** Form generated from reading UI file 'NMN.ui'
**
** Created by: Qt User Interface Compiler version 6.9.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NMN_H
#define UI_NMN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_NMN
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *NMN)
    {
        if (NMN->objectName().isEmpty())
            NMN->setObjectName("NMN");
        NMN->resize(800, 600);
        centralwidget = new QWidget(NMN);
        centralwidget->setObjectName("centralwidget");
        NMN->setCentralWidget(centralwidget);
        menubar = new QMenuBar(NMN);
        menubar->setObjectName("menubar");
        NMN->setMenuBar(menubar);
        statusbar = new QStatusBar(NMN);
        statusbar->setObjectName("statusbar");
        NMN->setStatusBar(statusbar);

        retranslateUi(NMN);

        QMetaObject::connectSlotsByName(NMN);
    } // setupUi

    void retranslateUi(QMainWindow *NMN)
    {
        NMN->setWindowTitle(QCoreApplication::translate("NMN", "NMN", nullptr));
    } // retranslateUi

};

namespace Ui {
    class NMN: public Ui_NMN {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NMN_H
