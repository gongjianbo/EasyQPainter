#pragma once
#include <QWidget>
#include <QTimer>

//展示画笔/画刷颜色的使用
class PenColor : public QWidget
{
    Q_OBJECT
public:
    explicit PenColor(QWidget *parent = nullptr);

protected:
    //显示时才启动定时动画
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    //绘制
    void paintEvent(QPaintEvent *event) override;

private:
    //当前角度偏移
    double angleOffset{0.0};
    //角度步进
    double angleStep{0.01};

    //定时移动
    QTimer timer;
};
