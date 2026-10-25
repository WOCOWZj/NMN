#pragma once

#include <QImage>

#include "core/Score.h"

namespace pipeline {

/// 把识别结果渲染回图片：盖住原谱面，用红字标出简谱。
/// 原 Converter 继承 QWidget 却从不显示，其实只是渲染器，现已剥离界面职责。
class ScoreRenderer
{
public:
    static QImage render(const QImage& source, const core::Score& score);
};

} // namespace pipeline
