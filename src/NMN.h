#pragma once
#include "ui_NMN.h"
#include <QMainWindow>

class NMN : public QMainWindow {
    Q_OBJECT
    
public:
    NMN(QWidget* parent = nullptr);
    ~NMN();

private:
    Ui_NMN* ui;
};