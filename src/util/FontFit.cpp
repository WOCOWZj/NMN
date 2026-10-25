#include "util/FontFit.h"

#include <QFontMetrics>

namespace util {

int fitPointSize(const QFont& base,
                 const QRect& reference,
                 const QString& text,
                 int maxSize,
                 int minSize)
{
    if (reference.width() <= 0 || reference.height() <= 0)
        return minSize;

    QFont font = base;
    for (int size = maxSize; size > minSize; --size)
    {
        font.setPointSize(size);
        const QFontMetrics metrics(font);
        const QRect bounds = metrics.boundingRect(reference, Qt::AlignCenter, text);
        if (bounds.width() <= reference.width() && bounds.height() <= reference.height())
            return size;
    }
    return minSize;
}

} // namespace util
