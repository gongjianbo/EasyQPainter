#ifndef MYPENSTYLE_H
#define MYPENSTYLE_H

#include <QWidget>

//展示画笔样式的使用
class MyPenStyle : public QWidget
{
    Q_OBJECT
public:
    explicit MyPenStyle(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    //CustomDashLine时使用，奇数为线长，偶数为间隔，绘制时循环使用
    QVector<qreal> customDash{2,3,4,5,6,7};
    //customDash的累计长度
    int dashCount=0;
    //蚂蚁线偏移
    int dashOffset=0;
};

#endif // MYPENSTYLE_H
