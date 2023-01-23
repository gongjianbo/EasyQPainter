#include "TabChart.h"
#include "ui_TabChart.h"

TabChart::TabChart(QWidget *parent)
    : QWidget{parent}
    , ui{new Ui::TabChart}
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
}

TabChart::~TabChart()
{
    delete ui;
}
