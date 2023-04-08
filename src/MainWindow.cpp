#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QIcon>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent}
    , ui{new Ui::MainWindow}
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/icon.ico"));
    ui->tabWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}
