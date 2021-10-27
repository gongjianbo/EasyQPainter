#pragma once
#include <QWidget>
#include <QTimer>

//绘制波形
class SineWave : public QWidget
{
    Q_OBJECT
public:
    explicit SineWave(QWidget *parent = nullptr);
    //绘制
    void paintEvent(QPaintEvent *event) override;

private:
    //定时器动画
    QTimer timer;
    //根据时间步进
    int timeVal{0}; 
};
