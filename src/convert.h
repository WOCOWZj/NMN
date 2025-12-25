#pragma once

#include <QObject>
#include <QList>
#include <QFile>
#include <QPainter>
#include <QPixmap>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>

#include "noteclass.h"

class Converter : public QWidget
{
    Q_OBJECT

public slots:
    void onResultReady(const QPixmap &image,const Note &tune, const QList<Note> &notes);

private:
    int tune2int;
    QPixmap origin_image;
    QList<Note> notes;
    QPixmap converted_image;
    void convert();
    void adaptFontSize(QPainter &painter, const QString &text);
    bool output(QWidget *parent = nullptr);
};