#include "MyPenColor.h"

#include <QPainter>
#include <QtMath>
#include <QLinearGradient>
#include <QTimer>

MyPenColor::MyPenColor(QWidget *parent)
    : QWidget(parent)
{
    //定时移动
    QTimer *timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,[=](){
        //使offset一直在[-1,1)区间内
        angleOffset+=angleStep*5; //*5加速移动
        if(angleOffset>1.0-angleStep/2)
            angleOffset=-1.0;
        update();
    });
    timer->start(150);
}

void MyPenColor::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    //先画一个黑底白框
    painter.fillRect(this->rect(),Qt::black);
    painter.setPen(QPen(Qt::white,1,Qt::DashLine));
    painter.drawRect(this->rect().adjusted(0,0,-1,-1)); //右下角会超出范围

    //计算路径
    QPainterPath line_path;
    double angle=angleOffset;
    line_path.moveTo(0,height()/2-height()/3*sin(angle*3.14159265));
    QList<int> x_pos;//竖线横坐标
    for(int i=0;i<width();i++,angle+=angleStep)
    {
        if(angle>1.0-angleStep/2){
            angle=-1.0;
        }
        if(abs(angle-0.5)<1E-7){
            x_pos.push_back(i);
        }
        //sin值域为[-1,1]，这里相当于把y值下移并放大
        //因为Qt是屏幕坐标系，所以又把y取反了再加的偏移
        line_path.lineTo(i,height()/2-height()/3*sin(angle*3.14159265));
    }

    //绘制路径
    painter.save();
    //划分为三个区域依次绘制
    painter.setRenderHint(QPainter::Antialiasing);
    //
    painter.setClipRect(0,0,width(),height()/3);
    painter.setPen(QPen(Qt::cyan,2));
    painter.drawPath(line_path);
    //
    painter.setClipRect(0,height()/3,width(),height()/3);
    painter.setPen(QPen(Qt::blue,2));
    painter.drawPath(line_path);
    //
    painter.setClipRect(0,height()/3*2,width(),height()/3);
    painter.setPen(QPen(Qt::yellow,2));
    painter.drawPath(line_path);
    painter.restore();

    //绘制渐变底色
    line_path.lineTo(width(),height());
    line_path.lineTo(0,height());
    line_path.closeSubpath();
    QLinearGradient gradient(0,0,0,height());
    gradient.setColorAt(0.0,QColor(0,255,0,150));
    gradient.setColorAt(1.0,QColor(0,255,0,0));
    painter.fillPath(line_path,gradient);

    //画两条分割线在上面
    painter.setPen(QPen(Qt::white,1,Qt::DashLine));
    painter.drawLine(0,height()/3,width(),height()/3);
    painter.drawLine(0,height()/3*2,width(),height()/3*2);
    //绘制竖线
    for(int i:x_pos){
        painter.drawLine(i,0,i,height());
    }
}
