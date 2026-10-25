#pragma once

#include <QRect>
#include <QString>

namespace core {

/// 一个简谱音符。纯数据 + 音高换算，不含任何 UI / OpenCV 依赖。
/// 枚举顺序与 NoteDialog 下拉框的索引一一对应，改动需同步界面。
struct Note
{
    enum class Name
    {
        Do = 0,
        Re,
        Mi,
        Fa,
        Sol,
        La,
        Ti
    };

    enum class Accidental
    {
        Sharp = 0,
        Natural,
        Flat
    };

    enum class OctaveDot
    {
        Lower = 0,
        None,
        Upper
    };

    /// 简谱用高低八度点只表达 ±1 个八度，超出此范围无法忠实记谱
    static constexpr int kMinSemitone = -12;
    static constexpr int kMaxSemitone = 23;

    QRect rect;
    Name name = Name::Do;
    Accidental accidental = Accidental::Natural;
    OctaveDot octaveDot = OctaveDot::None;

    /// 形如 " 1 "、"`3#"、".5b" —— 固定三字符，供等宽绘制
    QString toNumberedString() const;

    /// 相对中央组的半音序号，可为负
    int toSemitone() const;

    /// 由半音序号还原音符；超出可表示范围时钳制并记录告警
    static Note fromSemitone(int semitone);
};

} // namespace core
