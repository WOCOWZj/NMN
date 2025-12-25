#pragma once

#include <QObject>
#include <QList>

#include "noteclass.h"

class Converter
{
    Q_OBJECT

public slots:
    void onResultReady(const QList<Note> notes);

private:
    QList<Note> notes;
    void convert;
}