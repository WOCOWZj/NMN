#include "modification.h"

Modification::Modification(const QString &title, QWidget *parent, const Note &note, const Mode &Mode)
    : QDialog(parent)
{
    this->setWindowTitle(title);
    this->setModal(true);

    QVBoxLayout *vbox = new QVBoxLayout(this);
    QHBoxLayout *hbox1 = new QHBoxLayout(this);
    QHBoxLayout *hbox2 = new QHBoxLayout(this);
    QHBoxLayout *hbox3 = new QHBoxLayout(this);
    QHBoxLayout *hbox4 = new QHBoxLayout(this);

    this->notename_label = new QLabel("音名");
    this->notename_combobox = new QComboBox();
    this->notename_combobox->addItems({"1", "2", "3", "4", "5", "6", "7"});
    this->notename_combobox->setCurrentIndex(static_cast<int>(note.noteName));
    hbox1->addWidget(this->notename_label);
    hbox1->addWidget(this->notename_combobox);

    this->accidental_label = new QLabel("升降调");
    this->accidental_combobox = new QComboBox;
    this->accidental_combobox->addItems({"Sharp", "Natural", "Flat"});
    this->accidental_combobox->setCurrentIndex(static_cast<int>(note.accidental));
    hbox2->addWidget(this->accidental_label);
    hbox2->addWidget(this->accidental_combobox);

    this->octdot_label = new QLabel("音区");
    this->octdot_combobox = new QComboBox;
    this->octdot_combobox->addItems({"Lower", "None", "Higher"});
    this->octdot_combobox->setCurrentIndex(static_cast<int>(note.octDot));
    hbox3->addWidget(this->octdot_label);
    hbox3->addWidget(this->octdot_combobox);

    this->accept_button = new QPushButton("确定");
    this->cancel_button = new QPushButton("取消");
    hbox4->addStretch();
    hbox4->addWidget(this->accept_button);
    hbox4->addWidget(this->cancel_button);

    vbox->addLayout(hbox1);
    vbox->addLayout(hbox2);
    if (Mode == Mode::Mod8App)
        vbox->addLayout(hbox3);
    vbox->addLayout(hbox4);

    connect(this->accept_button, &QPushButton::clicked, this, &Modification::onAccept);
    connect(this->cancel_button, &QPushButton::clicked, this, &Modification::onCancel);

    this->accept_button->setDefault(true);
}

int Modification::exec()
{
    if (this->parentWidget())
        this->move(this->parentWidget()->window()->frameGeometry().center() - this->frameGeometry().center());
    return QDialog::exec();
}

void Modification::onAccept()
{
    Note note;
    note.noteName = static_cast<Note::NoteName>(this->notename_combobox->currentIndex());
    note.accidental = static_cast<Note::Accidental>(this->accidental_combobox->currentIndex());
    note.octDot = static_cast<Note::OctDot>(this->octdot_combobox->currentIndex());
    note.rect = QRect();
    this->result = note;
    this->accept();
}

void Modification::onCancel()
{
    this->reject();
}

Note Modification::getResult() const
{
    return this->result;
}