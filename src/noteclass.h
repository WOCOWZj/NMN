#pragma once

#include <QRect>
#include <QString>

struct Note
{
    QRect rect;
    enum class NoteName
    {
        C,
        D,
        E,
        F,
        G,
        A,
        B
    };
    enum class Accidental
    {
        SHARP,
        NATURAL,
        FLAT
    };
    enum class OctDot
    {
        LOWER,
        NONE,
        UPPER
    };
    NoteName noteName;
    Accidental accidental;
    OctDot octDot;

    QString Note2QString() const
    {
        QString res;
        switch (this->noteName)
        {
        case NoteName::C:
            res = "1";
            break;
        case NoteName::D:
            res = "2";
            break;
        case NoteName::E:
            res = "3";
            break;
        case NoteName::F:
            res = "4";
            break;
        case NoteName::G:
            res = "5";
            break;
        case NoteName::A:
            res = "6";
            break;
        case NoteName::B:
            res = "7";
            break;
        }
        switch (this->accidental)
        {
        case Accidental::SHARP:
            res += "#";
            break;
        case Accidental::FLAT:
            res += "b";
            break;
        case Accidental::NATURAL:
            res += " ";
            break;
        }
        switch (this->octDot)
        {
        case OctDot::UPPER:
            res = "`" + res;
            break;
        case OctDot::LOWER:
            res = "." + res;
            break;
        case OctDot::NONE:
            res = " " + res;
            break;
        }
        return res;
    }
};
