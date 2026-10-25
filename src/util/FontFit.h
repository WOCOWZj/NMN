#pragma once

#include <QFont>
#include <QRect>
#include <QString>

namespace util {

/// 求能塞进 reference 矩形的最大字号。
/// 原先 highlight.cpp 与 convert.cpp 各抄了一份 29 行实现，现统一到这里。
int fitPointSize(const QFont& base,
                 const QRect& reference,
                 const QString& text,
                 int maxSize = 12,
                 int minSize = 1);

} // namespace util
