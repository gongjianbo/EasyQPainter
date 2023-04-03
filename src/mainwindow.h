#pragma once
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

// 主窗口，包含各分组页面
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};
