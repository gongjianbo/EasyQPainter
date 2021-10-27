#pragma once
#include <QWidget>
#include <QTimer>

//calculate degree
//绘制圆盘，根据坐标位置计算角度
class CalcDegree : public QWidget
{
    Q_OBJECT
public:
    explicit CalcDegree(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    //画一个圆盘
    void drawCircle(QPainter *painter);
    //绘制一个椭圆轨迹的Item
    void drawItemPath(QPainter *painter);
    //绘制鼠标点到竖直正方向夹角
    void drawPosDegree(QPainter *painter);

private:
    //定时动画
    QTimer timer;
    //旋转的角度 [0-360]
    int theRotate{0};
    //鼠标位置
    QPoint mousePos{-1000, -1000};
};
