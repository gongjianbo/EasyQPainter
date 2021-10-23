#include "TabChart.h"
#include "ui_TabChart.h"

TabChart::TabChart(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabChart)
{
    ui->setupUi(this);
}

TabChart::~TabChart()
{
    delete ui;
}
