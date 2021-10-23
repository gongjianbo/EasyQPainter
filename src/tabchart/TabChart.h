#pragma once
#include <QWidget>

namespace Ui {
class TabChart;
}

class TabChart : public QWidget
{
    Q_OBJECT

public:
    explicit TabChart(QWidget *parent = nullptr);
    ~TabChart();

private:
    Ui::TabChart *ui;
};

