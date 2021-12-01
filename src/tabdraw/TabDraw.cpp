#include "TabDraw.h"
#include "ui_TabDraw.h"

TabDraw::TabDraw(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabDraw)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
}

TabDraw::~TabDraw()
{
    delete ui;
}
