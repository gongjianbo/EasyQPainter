#ifndef MYWAVE_H
#define MYWAVE_H

#include <QWidget>

//绘制波形
class MyWave : public QWidget
{
    Q_OBJECT
public:
    explicit MyWave(QWidget *parent = nullptr);
    //绘制
    void paintEvent(QPaintEvent *event) override;

private:
    //根据时间步进
    int timeVal=0;
};

#endif // MYWAVE_H
