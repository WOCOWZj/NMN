#pragma once

#include <QDialog>
#include <QObject>
#include <QLabel>
#include <QComboBox>
#include <QBoxlayout>
#include <QPushButton>
#include <QFile>

#include "noteclass.h"

class Modification : public QDialog
{
    Q_OBJECT

public:
    explicit Modification(const QString &title, QWidget *parent = nullptr, const Note &note = {.rect = QRect(), .noteName = Note::NoteName::C, .accidental = Note::Accidental::NATURAL, .octDot = Note::OctDot::NONE});
    int exec() override;
    Note getResult() const;

public slots:
    void onAccept();
    void onCancel();

private:
    QLabel *notename_label;
    QComboBox *notename_combobox;
    QLabel *accidental_label;
    QComboBox *accidental_combobox;
    QLabel *octdot_label;
    QComboBox *octdot_combobox;
    QPushButton *accept_button;
    QPushButton *cancel_button;
    Note result;
};
