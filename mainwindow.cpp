#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //默认显示最后一个，便于调试
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);

    setWindowTitle("Easy QPainter (By: GongJianBo 1992)");
    qDebug()<<"Easy QPainter \n"
              "展示 QPainter 的使用 \n"
              "作者：龚建波 \n"
              "QQ 交流群：647637553";
}

MainWindow::~MainWindow()
{
    delete ui;
}

