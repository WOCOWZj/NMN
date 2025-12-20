#include "NMN.h"

NMN::NMN(QWidget *parent)
    : QMainWindow(parent), ui(new Ui_NMN)
{
    ui->setupUi(this);
    this->setAcceptDrops(true);
}

NMN::~NMN()
{
    delete ui;
}

void NMN::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls() and event->mimeData()->urls().size() == 1)
    {
        QString filepath = event->mimeData()->urls().at(0).path();
        filepath = filepath.right(filepath.length() - 1);
        QPixmap pixmap(filepath);
        if (!pixmap.isNull())
            event->acceptProposedAction();
        else
            event->ignore();
    }
    else
        event->ignore();

    return;
}

void NMN::dropEvent(QDropEvent *event)
{
    QString filepath = event->mimeData()->urls().at(0).path();
    filepath = filepath.right(filepath.length() - 1);
    ui->infoLabel->setText(filepath);

    ui->scrollarea->setMinimumWidth(800);
    ui->imageLabel->setMinimumWidth(800);
    ui->scrollarea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->scrollarea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    QPixmap scaledPixmap = QPixmap(filepath).scaledToWidth(ui->imageLabel->width() - 40, Qt::SmoothTransformation);
    ui->imageLabel->setPixmap(scaledPixmap);

    this->path = filepath;
    emit ImageDropped(filepath);

    event->accept();

    return;
}
