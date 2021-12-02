#pragma once
#include <QWidget>
#include "XYAxis.h"

//笛卡尔坐标系(直角坐标系)图表的绘制
class XYView : public QWidget
{
    Q_OBJECT
public:
    explicit XYView(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
void resizeEvent(QResizeEvent *event) override;

public slots:
void refresh();

private:
    XYAxis *xAxis;
    XYAxis *yAxis;
};

