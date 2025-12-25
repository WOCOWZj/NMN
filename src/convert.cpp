#include "convert.h"

void Converter::onResultReady(const QPixmap &image, const Note &tune, const QList<Note> &notes)
{
    this->origin_image = image;
    this->notes = notes;
    this->tune2int = tune.Note2int();
    for (Note &note : this->notes)
    {
        Note newnote = Note::int2Note(tune2int + note.Note2int());
        newnote.rect = note.rect;
        note = newnote;
    }
    convert();
    output(this);
}

void Converter::convert()
{
    if (this->origin_image.isNull())
        return;
    this->converted_image = QPixmap(this->origin_image.size());
    this->converted_image.fill(Qt::white);

    QPainter painter(&this->converted_image);
    painter.drawPixmap(QPoint(0, 0), this->origin_image);

    painter.setBrush(Qt::white);
    painter.setPen(Qt::transparent);
    for (const Note &note : this->notes)
    {
        QRect rect = note.rect;
        rect.setTop(rect.top() + rect.height() / 8);
        rect.setHeight(rect.height() / 7 * 8);
        painter.drawRect(rect);
    }

    adaptFontSize(painter, "F");
    painter.setPen(Qt::red);
    for (const Note &note : this->notes)
    {
        const QString note_text = note.Note2QString();
        painter.drawText((note.rect.topLeft() + 3 * note.rect.bottomLeft()) / 4, note_text);
    }
}

bool Converter::output(QWidget *parent)
{
    if (this->converted_image.isNull())
    {
        QMessageBox::warning(parent, "警告", "无法保存空图像。");
        return false;
    }

    QString fileName = QFileDialog::getSaveFileName(
        parent,
        QObject::tr("保存图像"),
        "",
        QObject::tr("PNG 图像 (*.png);;JPEG 图像 (*.jpg *.jpeg);;BMP 图像 (*.bmp)"));

    if (fileName.isEmpty())
        return false;

    bool success = this->converted_image.save(fileName);
    if (!success)
        QMessageBox::critical(parent, "Error", "Failed to Save");
    else
        QMessageBox::information(parent, "Success", "Successfully Saved");

    return success;
}

void Converter::adaptFontSize(QPainter &painter, const QString &text)
{
    int sum_w = 0, sum_h = 0;
    for (const Note note : this->notes)
    {
        const QRect rect = note.rect;
        sum_w += rect.width();
        sum_h += rect.height();
    }
    int ave_w = sum_w / this->notes.length(), ave_h = sum_h / this->notes.length();
    QRect ave_rect = QRect(0, 0, ave_w, ave_h);

    QFont font = painter.font();
    int fontSize = 12;
    font.setPointSize(fontSize);
    painter.setFont(font);

    QFontMetrics fm(font);
    QRect br = fm.boundingRect(ave_rect, Qt::AlignCenter, text);

    while (fontSize > 1 && (br.width() > ave_rect.width() || br.height() > ave_rect.height()))
    {
        fontSize--;
        font.setPointSize(fontSize);
        painter.setFont(font);
        fm = QFontMetrics(font);
        br = fm.boundingRect(ave_rect, Qt::AlignCenter, text);
    }
}