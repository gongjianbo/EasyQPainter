#ifndef MYCALCULATEPOS_H
#define MYCALCULATEPOS_H

#include <QWidget>
#include <QImage>

//绘制圆盘，根据角度计算坐标位置
class MyCalculatePos : public QWidget
{
    Q_OBJECT
public:
    explicit MyCalculatePos(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void initImg_1();
    void initImg_2();
    void initImg_3();
    void draw_4(QPainter *painter);
    void draw_5(QPainter *painter,int rotate);

private:
    //旋转的角度 [0-360]
    int theRotate=0;
    //第一个图，为了旋转时文字不抖动，先绘制到画布
    //不过旋转时文字的抗锯齿效果很差
    QImage theImg_1;
    //图2 天干
    QImage theImg_2;
    //图3 地支
    QImage theImg_3;
};

#endif // MYCALCULATEPOS_H
