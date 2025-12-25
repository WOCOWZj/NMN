#pragma once

#include <QRect>
#include <QString>

struct Note
{
    QRect rect = QRect();
    enum class NoteName
    {
        Do,
        Re,
        Mi,
        Fa,
        Sol,
        La,
        Ti
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
    NoteName noteName = NoteName::Do;
    Accidental accidental = Accidental::NATURAL;
    OctDot octDot = OctDot::NONE;

    QString Note2QString() const
    {
        QString res;
        switch (this->noteName)
        {
        case NoteName::Do:
            res = "1";
            break;
        case NoteName::Re:
            res = "2";
            break;
        case NoteName::Mi:
            res = "3";
            break;
        case NoteName::Fa:
            res = "4";
            break;
        case NoteName::Sol:
            res = "5";
            break;
        case NoteName::La:
            res = "6";
            break;
        case NoteName::Ti:
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

    int Note2int() const
    {
        int res = 0;
        switch (this->noteName)
        {
        case NoteName::Do:
            res = 0;
            break;
        case NoteName::Re:
            res = 2;
            break;
        case NoteName::Mi:
            res = 4;
            break;
        case NoteName::Fa:
            res = 5;
            break;
        case NoteName::Sol:
            res = 7;
            break;
        case NoteName::La:
            res = 9;
            break;
        case NoteName::Ti:
            res = 11;
            break;
        }
        switch (this->accidental)
        {
        case Accidental::SHARP:
            res++;
            break;
        case Accidental::FLAT:
            res--;
            break;
        case Accidental::NATURAL:
            break;
        }
        switch (this->octDot)
        {
        case OctDot::UPPER:
            res += 12;
            break;
        case OctDot::LOWER:
            res -= 12;
            break;
        case OctDot::NONE:
            break;
        }
        return res;
    }

    static Note int2Note(int number)
    {
        Note res = Note();
        switch ((number % 12 + 12) % 12)
        {
        case 0:
            res.noteName = NoteName::Do;
            res.accidental = Accidental::NATURAL;
            break;
        case 1:
            res.noteName = NoteName::Do;
            res.accidental = Accidental::SHARP;
            break;
        case 2:
            res.noteName = NoteName::Re;
            res.accidental = Accidental::NATURAL;
            break;
        case 3:
            res.noteName = NoteName::Re;
            res.accidental = Accidental::SHARP;
            break;
        case 4:
            res.noteName = NoteName::Mi;
            res.accidental = Accidental::NATURAL;
            break;
        case 5:
            res.noteName = NoteName::Fa;
            res.accidental = Accidental::NATURAL;
            break;
        case 6:
            res.noteName = NoteName::Fa;
            res.accidental = Accidental::SHARP;
            break;
        case 7:
            res.noteName = NoteName::Sol;
            res.accidental = Accidental::NATURAL;
            break;
        case 8:
            res.noteName = NoteName::Sol;
            res.accidental = Accidental::SHARP;
            break;
        case 9:
            res.noteName = NoteName::La;
            res.accidental = Accidental::NATURAL;
            break;
        case 10:
            res.noteName = NoteName::La;
            res.accidental = Accidental::SHARP;
            break;
        case 11:
            res.noteName = NoteName::Ti;
            res.accidental = Accidental::NATURAL;
            break;
        }
        switch (static_cast<int>(floor(number / 12.0)))
        {
        case -1:
            res.octDot = OctDot::LOWER;
            break;
        case 0:
            res.octDot = OctDot::NONE;
            break;
        case 1:
            res.octDot = OctDot::UPPER;
            break;
        }
        return res;
    }
};
