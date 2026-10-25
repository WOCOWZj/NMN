#pragma once

#include <QColor>
#include <QImage>
#include <QLabel>
#include <QRect>
#include <QWidget>

#include "core/Score.h"

/// 带交互的谱面视图：显示底图与识别出的音符，支持悬停高亮、左键改、
/// 右键删、Shift 框选新增、回车设调号并提交。取代原 HighLightLabel。
class ScoreViewLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ScoreViewLabel(QWidget* parent = nullptr);

    void load(const QImage& image, const core::Score& score);

signals:
    void submitted(const core::Score& score);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    int indexAt(const QPoint& position) const;
    QRect averageNoteRect() const;

    void editNote(int index);
    void deleteNote(int index);
    void addNoteInSelection();
    void submit();

    QImage m_image;
    core::Score m_score;
    int m_spacing = 0;

    int m_hoveredIndex = -1;
    int m_pressedIndex = -1;
    bool m_leftPressed = false;
    bool m_rightPressed = false;

    bool m_selecting = false;
    QRect m_selection;

    const QColor m_outlineColor = Qt::red;
};

/// 承载 ScoreViewLabel 的容器窗口。取代原 HighLight。
class ScoreView : public QWidget
{
    Q_OBJECT

public:
    explicit ScoreView(QWidget* parent = nullptr);

    void load(const QImage& image, const core::Score& score);

signals:
    void submitted(const core::Score& score);

private:
    ScoreViewLabel* m_label = nullptr;
};
