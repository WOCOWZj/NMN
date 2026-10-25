#include "pipeline/ScoreRenderer.h"

#include "util/FontFit.h"
#include "util/Logger.h"

#include <QFont>
#include <QPainter>
#include <QPixmap>

namespace pipeline {

namespace {

/// 音符框的平均尺寸，用于推算合适的字号
QRect averageNoteRect(const core::Score& score)
{
    if (score.isEmpty())
        return QRect(0, 0, 1, 1);

    int sumWidth = 0;
    int sumHeight = 0;
    for (int i = 0; i < score.size(); ++i)
    {
        sumWidth += score.at(i).rect.width();
        sumHeight += score.at(i).rect.height();
    }

    return QRect(0, 0, sumWidth / score.size(), sumHeight / score.size());
}

} // namespace

QImage ScoreRenderer::render(const QImage& source, const core::Score& score)
{
    if (source.isNull())
    {
        util::Logger::warn(QStringLiteral("渲染失败：源图为空"));
        return QImage();
    }

    QImage canvas(source.size(), QImage::Format_ARGB32);
    canvas.fill(Qt::white);

    QPainter painter(&canvas);
    painter.drawImage(QPoint(0, 0), source);

    // 先用白底盖住原谱面上的符头
    painter.setBrush(Qt::white);
    painter.setPen(Qt::transparent);
    for (int i = 0; i < score.size(); ++i)
    {
        QRect rect = score.at(i).rect;
        rect.setTop(rect.top() + rect.height() / 8);
        rect.setHeight(rect.height() / 7 * 8);
        painter.drawRect(rect);
    }

    QFont font = painter.font();
    font.setPointSize(util::fitPointSize(font, averageNoteRect(score), QStringLiteral("F")));
    painter.setFont(font);

    painter.setPen(Qt::red);
    for (int i = 0; i < score.size(); ++i)
    {
        const core::Note& note = score.at(i);
        painter.drawText((note.rect.topLeft() + 3 * note.rect.bottomLeft()) / 4,
                         note.toNumberedString());
    }

    painter.end();
    return canvas;
}

} // namespace pipeline
