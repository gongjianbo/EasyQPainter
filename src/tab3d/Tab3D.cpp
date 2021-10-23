#include "Tab3D.h"
#include "ui_Tab3D.h"

Tab3D::Tab3D(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Tab3D)
{
    ui->setupUi(this);
}

Tab3D::~Tab3D()
{
    delete ui;
}
