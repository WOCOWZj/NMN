#include "core/Note.h"

#include "util/Logger.h"

#include <QtGlobal>
#include <cmath>

namespace core {

QString Note::toNumberedString() const
{
    QString result;
    switch (name)
    {
    case Name::Do:
        result = QStringLiteral("1");
        break;
    case Name::Re:
        result = QStringLiteral("2");
        break;
    case Name::Mi:
        result = QStringLiteral("3");
        break;
    case Name::Fa:
        result = QStringLiteral("4");
        break;
    case Name::Sol:
        result = QStringLiteral("5");
        break;
    case Name::La:
        result = QStringLiteral("6");
        break;
    case Name::Ti:
        result = QStringLiteral("7");
        break;
    }

    switch (accidental)
    {
    case Accidental::Sharp:
        result += QLatin1Char('#');
        break;
    case Accidental::Flat:
        result += QLatin1Char('b');
        break;
    case Accidental::Natural:
        result += QLatin1Char(' ');
        break;
    }

    switch (octaveDot)
    {
    case OctaveDot::Upper:
        result = QLatin1Char('`') + result;
        break;
    case OctaveDot::Lower:
        result = QLatin1Char('.') + result;
        break;
    case OctaveDot::None:
        result = QLatin1Char(' ') + result;
        break;
    }

    return result;
}

int Note::toSemitone() const
{
    int semitone = 0;
    switch (name)
    {
    case Name::Do:
        semitone = 0;
        break;
    case Name::Re:
        semitone = 2;
        break;
    case Name::Mi:
        semitone = 4;
        break;
    case Name::Fa:
        semitone = 5;
        break;
    case Name::Sol:
        semitone = 7;
        break;
    case Name::La:
        semitone = 9;
        break;
    case Name::Ti:
        semitone = 11;
        break;
    }

    switch (accidental)
    {
    case Accidental::Sharp:
        ++semitone;
        break;
    case Accidental::Flat:
        --semitone;
        break;
    case Accidental::Natural:
        break;
    }

    switch (octaveDot)
    {
    case OctaveDot::Upper:
        semitone += 12;
        break;
    case OctaveDot::Lower:
        semitone -= 12;
        break;
    case OctaveDot::None:
        break;
    }

    return semitone;
}

Note Note::fromSemitone(int semitone)
{
    int value = semitone;

    // 修复：原实现直接 floor(n/12) 后只匹配 -1/0/1，越界时静默保留默认八度，
    // 导致移调结果无声出错。此处显式钳制并留痕。
    if (value < kMinSemitone || value > kMaxSemitone)
    {
        const int clamped = qBound(static_cast<int>(kMinSemitone), value, static_cast<int>(kMaxSemitone));
        util::Logger::warn(QStringLiteral("音高 %1 超出简谱可表示范围 [%2, %3]，已钳制为 %4")
                               .arg(value)
                               .arg(kMinSemitone)
                               .arg(kMaxSemitone)
                               .arg(clamped));
        value = clamped;
    }

    Note note;

    switch ((value % 12 + 12) % 12)
    {
    case 0:
        note.name = Name::Do;
        note.accidental = Accidental::Natural;
        break;
    case 1:
        note.name = Name::Do;
        note.accidental = Accidental::Sharp;
        break;
    case 2:
        note.name = Name::Re;
        note.accidental = Accidental::Natural;
        break;
    case 3:
        note.name = Name::Re;
        note.accidental = Accidental::Sharp;
        break;
    case 4:
        note.name = Name::Mi;
        note.accidental = Accidental::Natural;
        break;
    case 5:
        note.name = Name::Fa;
        note.accidental = Accidental::Natural;
        break;
    case 6:
        note.name = Name::Fa;
        note.accidental = Accidental::Sharp;
        break;
    case 7:
        note.name = Name::Sol;
        note.accidental = Accidental::Natural;
        break;
    case 8:
        note.name = Name::Sol;
        note.accidental = Accidental::Sharp;
        break;
    case 9:
        note.name = Name::La;
        note.accidental = Accidental::Natural;
        break;
    case 10:
        note.name = Name::La;
        note.accidental = Accidental::Sharp;
        break;
    case 11:
        note.name = Name::Ti;
        note.accidental = Accidental::Natural;
        break;
    }

    switch (static_cast<int>(std::floor(value / 12.0)))
    {
    case -1:
        note.octaveDot = OctaveDot::Lower;
        break;
    case 0:
        note.octaveDot = OctaveDot::None;
        break;
    case 1:
        note.octaveDot = OctaveDot::Upper;
        break;
    default:
        // 已被钳制到 [-12, 23]，理论上不可达
        note.octaveDot = OctaveDot::None;
        break;
    }

    return note;
}

} // namespace core
