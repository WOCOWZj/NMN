#include "highlight.h"

HighLight::HighLight(QWidget *parent)
    : QWidget(parent)
{
    this->label = new HighLightLabel(this);
    this->label->setParent(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(this->label);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}

void HighLight::onImageProcessed(const QPixmap &pixmap, const QList<Note> &notes)
{
    this->label->init(pixmap, notes);
    resize(label->size());
    show();
    update();
    this->label->setFocus();
}

HighLightLabel::HighLightLabel(QWidget *parent)
    : QLabel(parent)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    connect(this, &HighLightLabel::LeftClick, this, &HighLightLabel::onLeftClick);
    connect(this, &HighLightLabel::RightClick, this, &HighLightLabel::onRightClick);
    connect(this, &HighLightLabel::Framed, this, &HighLightLabel::onFramed);
}

void HighLightLabel::init(const QPixmap &pixmap, const QList<Note> &notes)
{
    this->pixmap = pixmap;
    this->notes = notes;
    this->setFixedSize(this->pixmap.size());
    this->setAlignment(Qt::AlignCenter);

    int sum_w = 0, sum_h = 0;
    int max_h = 0;
    for (const Note note : this->notes)
    {
        const QRect rect = note.rect;
        sum_w += rect.width();
        sum_h += rect.height();
        max_h = qMax(max_h, rect.height());
    }
    int ave_w = sum_w / this->notes.length(), ave_h = sum_h / this->notes.length();
    this->ave_rect = QRect(0, 0, ave_w, ave_h);
    this->spacing = qRound(1.5 * max_h);
}

void HighLightLabel::adaptFontSize(QPainter &painter, const QRect &rect, const QString &text)
{
    QFont font = painter.font();
    int fontSize = 12;
    font.setPointSize(fontSize);
    painter.setFont(font);

    QFontMetrics fm(font);
    QRect br = fm.boundingRect(rect, Qt::AlignCenter, text);

    while (fontSize > 1 && (br.width() > rect.width() || br.height() > rect.height()))
    {
        fontSize--;
        font.setPointSize(fontSize);
        painter.setFont(font);
        fm = QFontMetrics(font);
        br = fm.boundingRect(rect, Qt::AlignCenter, text);
    }
}

void HighLightLabel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(QPen(this->outline_color));
    adaptFontSize(painter, this->ave_rect, "F");
    painter.drawPixmap(QPoint(0, 0), this->pixmap);

    for (const Note note : notes)
    {
        QString note_text = note.Note2QString();
        painter.drawText(note.rect.translated(0, spacing).topLeft(), note_text);
    }

    if (this->hovered_ind != -1)
        painter.drawRect(this->notes.at(this->hovered_ind).rect);

    if (selecting)
        painter.drawRect(this->selecting_rect.normalized());
}

void HighLightLabel::mouseMoveEvent(QMouseEvent *event)
{
    QPoint Pos = event->pos();
    if (this->selecting)
    {
        this->selecting_rect.setBottomRight(Pos);
        update();
    }
    else
    {
        int new_hovered_ind = -1;
        for (int i = 0; i < this->notes.length(); i++)
            if (notes.at(i).rect.contains(Pos))
            {
                new_hovered_ind = i;
                break;
            }

        if (this->hovered_ind != new_hovered_ind)
        {
            this->hovered_ind = new_hovered_ind;
            update();
        }
    }
    QLabel::mouseMoveEvent(event);
}

void HighLightLabel::leaveEvent(QEvent *event)
{
    this->hovered_ind = -1;
    this->press_ind = -1;
    this->release_ind = -1;
    this->selecting = false;
    QLabel::leaveEvent(event);
}

void HighLightLabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton or event->button() == Qt::RightButton)
    {
        QPoint Pos = event->pos();
        if (event->modifiers() & Qt::ShiftModifier)
        {
            this->selecting = true;
            this->selecting_rect.setTopLeft(Pos);
            setCursor(Qt::CrossCursor);
            update();
            this->press_ind = -1;
        }
        else
        {
            this->press_ind = -1;
            this->leftbutton_pressed = (event->button() == Qt::LeftButton);
            this->rightbutton_pressed = (event->button() == Qt::RightButton);
            for (int i = 0; i < this->notes.length(); i++)
                if (notes.at(i).rect.contains(Pos))
                {
                    this->press_ind = i;
                    break;
                }
        }
    }
    QLabel::mousePressEvent(event);
}

void HighLightLabel::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton or event->button() == Qt::RightButton)
    {
        if (this->selecting)
        {
            this->release_ind = -1;
            if (event->button() == Qt::LeftButton)
            {
                this->selecting = false;
                emit Framed();
            }
        }
        else if (press_ind != -1)
        {
            QPoint Pos = event->pos();
            this->release_ind = -1;
            for (int i = 0; i < this->notes.length(); i++)
                if (notes.at(i).rect.contains(Pos))
                {
                    this->release_ind = i;
                    break;
                }
            if (this->release_ind == this->press_ind)
            {
                int ind = this->press_ind;
                if (event->button() == Qt::LeftButton and this->leftbutton_pressed == true)
                {
                    emit LeftClick(ind);
                    this->leftbutton_pressed = false;
                }
                if (event->button() == Qt::RightButton and this->rightbutton_pressed == true)
                {
                    emit RightClick(ind);
                    this->rightbutton_pressed = false;
                }
            }
            this->press_ind = -1;
            this->release_ind = -1;
        }
    }
    QLabel::mouseReleaseEvent(event);
}

void HighLightLabel::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return)
    {
        QMessageBox::StandardButton reply = QMessageBox::question(nullptr, "提交", "确定要提交吗");
        if (reply == QMessageBox::Yes)
        {
            emit ResultReady(this->notes);
            this->parentWidget()->close();
        }
    }
    QLabel::keyPressEvent(event);
}

void HighLightLabel::onLeftClick(int ind)
{
    {
        QFile logFile("NMN.log");
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            QTextStream out(&logFile);
            out << QString("modifying ind %1, %2 (%3 %4 %5)\n")
                       .arg(ind)
                       .arg(notes.at(ind).Note2QString())
                       .arg(static_cast<int>(this->notes.at(ind).noteName))
                       .arg(static_cast<int>(this->notes.at(ind).accidental))
                       .arg(static_cast<int>(this->notes.at(ind).octDot));
        }
        logFile.close();
    }
    Modification modif("修改", this, this->notes.at(ind));
    if (modif.exec() == QDialog::Accepted)
    {
        Note note = modif.getResult();
        {
            QFile logFile("NMN.log");
            if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
            {
                QTextStream out(&logFile);
                out << QString("modified note: %1\n").arg(note.Note2QString());
            }
            logFile.close();
        }
        note.rect = this->notes.at(ind).rect;
        this->notes.replace(ind, note);
    }
    update();
}

void HighLightLabel::onRightClick(int ind)
{
    QMessageBox::StandardButton reply = QMessageBox::question(nullptr, "删除", "确定要删除吗");
    if (reply == QMessageBox::Yes)
        this->notes.removeAt(ind);
}

void HighLightLabel::onFramed()
{
    Modification modif("添加", this);
    if (modif.exec() == QDialog::Accepted)
    {
        Note note = modif.getResult();
        {
            QFile logFile("NMN.log");
            if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
            {
                QTextStream out(&logFile);
                out << QString("append note: %1\n").arg(note.Note2QString());
            }
            logFile.close();
        }
        note.rect = this->selecting_rect;
        this->notes.append(note);
    }
    update();
}
