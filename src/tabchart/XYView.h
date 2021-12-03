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
void mousePressEvent(QMouseEvent *event) override;
void mouseMoveEvent(QMouseEvent *event) override;
void mouseReleaseEvent(QMouseEvent *event) override;
void leaveEvent(QEvent *event) override;
void wheelEvent(QWheelEvent *event) override;

private:
//查找对应值下标
int searchDataIndex(int start, int end, double distinction) const;

public slots:
void refresh();

private:
//坐标轴
    XYAxis *xAxis;
    XYAxis *yAxis;
    //绘图区域，坐标轴在其边上
    QRect contentArea;
    //去掉坐标轴的图表区域
    QRect plotArea;
    //鼠标位置
    QPoint mousePos;//当前位置
    QPoint prevPos;//上次位置
    //鼠标按下
    bool pressFlag{false};

    //曲线xy值
    struct Node{
        int x;
        int y;
    };
    QVector<Node> seriesData;
};

