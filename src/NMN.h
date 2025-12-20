#pragma once
#include "NMN_autogen/include/ui_NMN.h"
#include <QMainWindow>
#include <QLabel>
#include <opencv2/opencv.hpp>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QFileInfo>
#include <QMessageBox>
#include <QPixmap>
#include <QString>
#include <qminmax.h>

class NMN : public QMainWindow
{
    Q_OBJECT

public:
    NMN(QWidget *parent = nullptr);
    ~NMN();
    QString path;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    Ui_NMN *ui;

signals:
    void ImageDropped(const QString filePath);
};