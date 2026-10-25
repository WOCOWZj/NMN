#include "ui/NoteDialog.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

NoteDialog::NoteDialog(const QString& title,
                       Mode mode,
                       const core::Note& note,
                       QWidget* parent)
    : QDialog(parent)
    , m_mode(mode)
    , m_result(note)
{
    setWindowTitle(title);
    setModal(true);

    auto* root = new QVBoxLayout(this);

    auto* nameRow = new QHBoxLayout;
    auto* nameLabel = new QLabel(QStringLiteral("音名"), this);
    m_nameBox = new QComboBox(this);
    m_nameBox->addItems({QStringLiteral("1"), QStringLiteral("2"), QStringLiteral("3"),
                         QStringLiteral("4"), QStringLiteral("5"), QStringLiteral("6"),
                         QStringLiteral("7")});
    m_nameBox->setCurrentIndex(static_cast<int>(note.name));
    nameRow->addWidget(nameLabel);
    nameRow->addWidget(m_nameBox);

    auto* accidentalRow = new QHBoxLayout;
    auto* accidentalLabel = new QLabel(QStringLiteral("升降调"), this);
    m_accidentalBox = new QComboBox(this);
    m_accidentalBox->addItems({QStringLiteral("Sharp"), QStringLiteral("Natural"), QStringLiteral("Flat")});
    m_accidentalBox->setCurrentIndex(static_cast<int>(note.accidental));
    accidentalRow->addWidget(accidentalLabel);
    accidentalRow->addWidget(m_accidentalBox);

    auto* octaveRow = new QHBoxLayout;
    auto* octaveLabel = new QLabel(QStringLiteral("音区"), this);
    m_octaveBox = new QComboBox(this);
    m_octaveBox->addItems({QStringLiteral("Lower"), QStringLiteral("None"), QStringLiteral("Higher")});
    m_octaveBox->setCurrentIndex(static_cast<int>(note.octaveDot));
    octaveRow->addWidget(octaveLabel);
    octaveRow->addWidget(m_octaveBox);

    auto* buttonRow = new QHBoxLayout;
    auto* acceptButton = new QPushButton(QStringLiteral("确定"), this);
    auto* cancelButton = new QPushButton(QStringLiteral("取消"), this);
    buttonRow->addStretch();
    buttonRow->addWidget(acceptButton);
    buttonRow->addWidget(cancelButton);

    root->addLayout(nameRow);
    root->addLayout(accidentalRow);
    if (m_mode == Mode::Edit)
        root->addLayout(octaveRow);
    root->addLayout(buttonRow);

    connect(acceptButton, &QPushButton::clicked, this, &NoteDialog::onAccepted);
    connect(cancelButton, &QPushButton::clicked, this, &NoteDialog::reject);
    acceptButton->setDefault(true);
}

int NoteDialog::exec()
{
    if (QWidget* owner = parentWidget())
        move(owner->window()->frameGeometry().center() - frameGeometry().center());
    return QDialog::exec();
}

void NoteDialog::onAccepted()
{
    core::Note note;
    note.name = static_cast<core::Note::Name>(m_nameBox->currentIndex());
    note.accidental = static_cast<core::Note::Accidental>(m_accidentalBox->currentIndex());
    note.octaveDot = static_cast<core::Note::OctaveDot>(m_octaveBox->currentIndex());
    note.rect = QRect();
    m_result = note;
    accept();
}
