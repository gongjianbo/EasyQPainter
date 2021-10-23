#include "TabDraw.h"
#include "ui_TabDraw.h"

TabDraw::TabDraw(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabDraw)
{
    ui->setupUi(this);
}

TabDraw::~TabDraw()
{
    delete ui;
}
