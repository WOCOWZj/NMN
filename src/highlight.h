#pragma once

#include <QLabel>
#include <QObject>
#include <QList>
#include <QRect>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QFont>
#include <qminmax.h>
#include <QColor>
#include <QWindow>
#include <QVBoxLayout>
#include <QString>
#include <QFile>
#include <QMessageBox>

#include "noteclass.h"
#include "modification.h"

class HighLightLabel : public QLabel
{
    Q_OBJECT

private:
    QPixmap pixmap;
    QList<Note> notes;
    void adaptFontSize(QPainter &painter, const QString &text);
    int spacing;
    const QColor outline_color = Qt::red;

    int hovered_ind = -1;

    int press_ind = -1;
    int release_ind = -1;
    bool leftbutton_pressed = false;
    bool rightbutton_pressed = false;

    bool selecting = false;
    QRect selecting_rect = QRect();

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

public:
    HighLightLabel(QWidget *parent = nullptr);
    void init(const QPixmap &pixmap, const QList<Note> &notes);

signals:
    void LeftClick(int ind);
    void RightClick(int ind);
    void Framed();
    void ResultReady(const QPixmap &image, const Note &tune, const QList<Note> &notes);

public slots:
    void onLeftClick(int ind);
    void onRightClick(int ind);
    void onFramed();
};

class HighLight : public QWidget
{
    Q_OBJECT

public:
    HighLightLabel *label;
    HighLight(QWidget *parent = nullptr);

public slots:
    void onImageProcessed(const QPixmap &pixmap, const QList<Note> &notes);
};
