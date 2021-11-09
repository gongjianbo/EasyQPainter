#pragma once
#include <QWidget>
#include <QTimer>
#include <QVector>

//展示画笔样式的使用
class PenStyle : public QWidget
{
    Q_OBJECT
public:
    explicit PenStyle(QWidget *parent = nullptr);

protected:
    //显示时才启动定时动画
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    //绘制
    void paintEvent(QPaintEvent *event) override;

private:
    //CustomDashLine时使用，奇数为线长，偶数为间隔，绘制时循环使用
    QVector<qreal> customDash{2, 3, 4, 5, 6, 7};
    //customDash的累计长度
    int dashCount{0};

    //虚线定时移动实现蚂蚁线
    QTimer timer;
    //蚂蚁线偏移
    int dashOffset{0};
};
