#include "Tab3D.h"
#include "ui_Tab3D.h"

Tab3D::Tab3D(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Tab3D)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
}

Tab3D::~Tab3D()
{
    delete ui;
}
