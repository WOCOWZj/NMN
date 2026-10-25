#include "ui/ScoreView.h"

#include "ui/NoteDialog.h"
#include "util/FontFit.h"
#include "util/Logger.h"

#include <QKeyEvent>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QVBoxLayout>

// ---------------------------------------------------------------------------
// ScoreView
// ---------------------------------------------------------------------------

ScoreView::ScoreView(QWidget* parent)
    : QWidget(parent)
    , m_label(new ScoreViewLabel(this))
{
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(m_label);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    connect(m_label, &ScoreViewLabel::submitted, this, &ScoreView::submitted);
}

void ScoreView::load(const QImage& image, const core::Score& score)
{
    m_label->load(image, score);
    resize(m_label->size());
    show();
    m_label->setFocus();
}

// ---------------------------------------------------------------------------
// ScoreViewLabel
// ---------------------------------------------------------------------------

ScoreViewLabel::ScoreViewLabel(QWidget* parent)
    : QLabel(parent)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

void ScoreViewLabel::load(const QImage& image, const core::Score& score)
{
    m_image = image;
    m_score = score;

    setFixedSize(m_image.size());
    setAlignment(Qt::AlignCenter);

    int maxHeight = 0;
    for (int i = 0; i < m_score.size(); ++i)
        maxHeight = qMax(maxHeight, m_score.at(i).rect.height());
    m_spacing = qRound(1.5 * maxHeight);

    m_hoveredIndex = -1;
    m_pressedIndex = -1;
    m_selecting = false;
    update();
}

QRect ScoreViewLabel::averageNoteRect() const
{
    if (m_score.isEmpty())
        return QRect(0, 0, 1, 1);

    int sumWidth = 0;
    int sumHeight = 0;
    for (int i = 0; i < m_score.size(); ++i)
    {
        sumWidth += m_score.at(i).rect.width();
        sumHeight += m_score.at(i).rect.height();
    }

    return QRect(0, 0, sumWidth / m_score.size(), sumHeight / m_score.size());
}

int ScoreViewLabel::indexAt(const QPoint& position) const
{
    for (int i = 0; i < m_score.size(); ++i)
    {
        if (m_score.at(i).rect.contains(position))
            return i;
    }
    return -1;
}

void ScoreViewLabel::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setPen(QPen(m_outlineColor));

    QFont font = painter.font();
    font.setPointSize(util::fitPointSize(font, averageNoteRect(), QStringLiteral("F")));
    painter.setFont(font);

    painter.drawImage(QPoint(0, 0), m_image);

    for (int i = 0; i < m_score.size(); ++i)
    {
        const core::Note& note = m_score.at(i);
        painter.drawText(note.rect.translated(0, m_spacing).topLeft(), note.toNumberedString());
    }

    if (m_hoveredIndex >= 0 && m_hoveredIndex < m_score.size())
        painter.drawRect(m_score.at(m_hoveredIndex).rect);

    if (m_selecting)
        painter.drawRect(m_selection.normalized());
}

void ScoreViewLabel::mouseMoveEvent(QMouseEvent* event)
{
    const QPoint position = event->pos();

    if (m_selecting)
    {
        m_selection.setBottomRight(position);
        update();
    }
    else
    {
        const int hovered = indexAt(position);
        if (hovered != m_hoveredIndex)
        {
            m_hoveredIndex = hovered;
            update();
        }
    }

    QLabel::mouseMoveEvent(event);
}

void ScoreViewLabel::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
    {
        const QPoint position = event->pos();

        if (event->modifiers() & Qt::ShiftModifier)
        {
            m_selecting = true;
            m_selection.setTopLeft(position);
            m_selection.setBottomRight(position);
            setCursor(Qt::CrossCursor);
            m_pressedIndex = -1;
            update();
        }
        else
        {
            m_pressedIndex = indexAt(position);
            m_leftPressed = (event->button() == Qt::LeftButton);
            m_rightPressed = (event->button() == Qt::RightButton);
        }
    }

    QLabel::mousePressEvent(event);
}

void ScoreViewLabel::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
    {
        if (m_selecting)
        {
            if (event->button() == Qt::LeftButton)
            {
                m_selecting = false;
                unsetCursor();
                addNoteInSelection();
            }
        }
        else if (m_pressedIndex != -1)
        {
            const int index = m_pressedIndex;
            m_pressedIndex = -1;

            if (indexAt(event->pos()) == index)
            {
                if (event->button() == Qt::LeftButton && m_leftPressed)
                {
                    m_leftPressed = false;
                    editNote(index);
                }
                else if (event->button() == Qt::RightButton && m_rightPressed)
                {
                    m_rightPressed = false;
                    deleteNote(index);
                }
            }
        }
    }

    QLabel::mouseReleaseEvent(event);
}

void ScoreViewLabel::leaveEvent(QEvent* event)
{
    m_hoveredIndex = -1;
    m_pressedIndex = -1;
    m_selecting = false;
    QLabel::leaveEvent(event);
}

void ScoreViewLabel::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        submit();
        return;
    }

    QLabel::keyPressEvent(event);
}

void ScoreViewLabel::editNote(int index)
{
    if (index < 0 || index >= m_score.size())
        return;

    const core::Note current = m_score.at(index);
    util::Logger::info(QStringLiteral("修改第 %1 个音符，原值 %2").arg(index).arg(current.toNumberedString()));

    NoteDialog dialog(QStringLiteral("修改"), NoteDialog::Mode::Edit, current, this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    core::Note updated = dialog.result();
    updated.rect = current.rect;
    m_score.replace(index, updated);

    util::Logger::info(QStringLiteral("修改后：%1").arg(updated.toNumberedString()));
    update();
}

void ScoreViewLabel::deleteNote(int index)
{
    if (index < 0 || index >= m_score.size())
        return;

    const auto reply = QMessageBox::question(this, QStringLiteral("删除"), QStringLiteral("确定要删除吗"));
    if (reply != QMessageBox::Yes)
        return;

    m_score.removeAt(index);
    m_hoveredIndex = -1;
    util::Logger::info(QStringLiteral("删除音符，剩余 %1 个").arg(m_score.size()));
    update();
}

void ScoreViewLabel::addNoteInSelection()
{
    if (m_selection.normalized().isEmpty())
        return;

    NoteDialog dialog(QStringLiteral("添加"), NoteDialog::Mode::Edit, core::Note(), this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    core::Note note = dialog.result();
    note.rect = m_selection.normalized();
    m_score.append(note);

    util::Logger::info(QStringLiteral("新增音符：%1").arg(note.toNumberedString()));
    update();
}

void ScoreViewLabel::submit()
{
    NoteDialog dialog(QStringLiteral("调号"), NoteDialog::Mode::Tonic, m_score.tonic(), this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    const core::Note tonic = dialog.result();
    util::Logger::info(QStringLiteral("调号：%1").arg(tonic.toNumberedString()));

    const auto reply = QMessageBox::question(this, QStringLiteral("提交"), QStringLiteral("确定要提交吗"));
    if (reply != QMessageBox::Yes)
        return;

    m_score.setTonic(tonic);
    emit submitted(m_score);

    if (QWidget* owner = parentWidget())
        owner->close();
}
