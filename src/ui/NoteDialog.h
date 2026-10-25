#pragma once

#include <QDialog>
#include <QString>

#include "core/Note.h"

class QComboBox;

/// 音符录入对话框。取代原 modification.cpp。
/// Mode::Edit 用于改写单个音符（含音区），Mode::Tonic 用于设定调号（隐藏音区）。
class NoteDialog : public QDialog
{
    Q_OBJECT

public:
    enum class Mode
    {
        Edit,
        Tonic
    };

    explicit NoteDialog(const QString& title,
                        Mode mode = Mode::Edit,
                        const core::Note& note = core::Note(),
                        QWidget* parent = nullptr);

    int exec() override;

    core::Note result() const { return m_result; }

private slots:
    void onAccepted();

private:
    Mode m_mode;
    core::Note m_result;
    QComboBox* m_nameBox = nullptr;
    QComboBox* m_accidentalBox = nullptr;
    QComboBox* m_octaveBox = nullptr;
};
