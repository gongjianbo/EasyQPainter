#pragma once
#include <QWidget>
#include <QPainterPath>
#include <QTimer>

//动画效果
class Animation : public QWidget
{
    Q_OBJECT
public:
    explicit Animation(QWidget *parent = nullptr);

protected:
    //显示时才启动定时动画
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    //绘制
    void paintEvent(QPaintEvent *event) override;

private:
    //1.定时器动画
    //1-1.波纹
    QTimer waveTimer;
    int waveOffset{0};
    const int waveLimit{100};
};
