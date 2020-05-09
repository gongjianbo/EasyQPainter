#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Easy QPainter (By: GongJianBo 1992)");
}

MainWindow::~MainWindow()
{
    delete ui;
}

