#include "NMN.h"

NMN::NMN(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui_NMN)
{
    ui->setupUi(this);
}

NMN::~NMN()
{
    delete ui; 
}