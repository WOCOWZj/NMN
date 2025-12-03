#pragma once
#include <QWindow>
#include <QRect>

class Marker : public QObject
{
    Q_OBJECT

private:
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
            FLAT,
            NATURAL
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
    };
    QList<Note> notes;

    struct KeySign
    {
        QRect rect;
        enum class Key
        {
            C,
            Cs,
            D,
            Ds,
            E,
            Es,
            F,
            Fs,
            G,
            Gs,
            A,
            As,
            B,
            Bs
        };
        Key key;
    };
    KeySign keySign;
};