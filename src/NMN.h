#pragma once
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
#include <QPainter>
#include <QList>
#include <QRect>

#include "NMN_autogen/include/ui_NMN.h"
#include "noteclass.h"

class NMN : public QMainWindow
{
    Q_OBJECT

public:
    explicit NMN(QWidget *parent = nullptr);
    ~NMN();

private:
    QPixmap image;
    QList<Note> rects;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    Ui_NMN *ui;

signals:
    void ImageDropped(const QString &filePath);
};
