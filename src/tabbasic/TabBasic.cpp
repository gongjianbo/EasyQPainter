#include "TabBasic.h"
#include "ui_TabBasic.h"

TabBasic::TabBasic(QWidget *parent)
    : QWidget{parent}
    , ui{new Ui::TabBasic}
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
}

TabBasic::~TabBasic()
{
    delete ui;
}
