#include "MyPenColor.h"

#include <QPainter>
#include <QPainterPath>
#include <QtMath>
#include <QLinearGradient>
#include <QConicalGradient>
#include <QTimer>

MyPenColor::MyPenColor(QWidget *parent)
    : QWidget(parent)
{
    //定时移动
    QTimer *timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,[=](){
        //使offset一直在[-1,1)区间内
        angleOffset+=angleStep;
        if(angleOffset>1.0-angleStep/2)
            angleOffset=-1.0;

        if(isHidden())
            return;
        update();
    });
    timer->start(50);
}

void MyPenColor::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    //先画一个黑底白框
    painter.fillRect(this->rect(),Qt::black);
    painter.setPen(QPen(Qt::white,1,Qt::DashLine));
    painter.drawRect(this->rect().adjusted(0,0,-1,-1)); //右下角会超出范围

    //【1】sin正弦曲线
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

    //先绘制渐变底色
    QPainterPath area_path=line_path;
    area_path.lineTo(width(),height());
    area_path.lineTo(0,height());
    area_path.closeSubpath();
    QLinearGradient gradient(0,0,0,height());
    gradient.setColorAt(0.0,QColor(0,255,0,150));
    gradient.setColorAt(1.0,QColor(0,255,0,0));
    painter.fillPath(area_path,gradient);

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

    //画两条分割线在上面
    painter.setPen(QPen(Qt::white,1,Qt::DashLine));
    painter.drawLine(0,height()/3,width(),height()/3);
    painter.drawLine(0,height()/3*2,width(),height()/3*2);
    //绘制竖线
    for(int i:x_pos){
        painter.drawLine(i,0,i,height());
    }

    //【2】雷达扫描
    //黑底白框圆盘
    const QRect scan_rect(10,10,100,100);//100*100
    const int scan_radius=scan_rect.width()/2;
    painter.save();
    painter.translate(scan_rect.center());
    QPainterPath ellipse_path;
    ellipse_path.addEllipse(QPoint(0,0),scan_radius,scan_radius);
    painter.fillPath(ellipse_path,Qt::black);
    painter.setPen(QPen(Qt::white,2));
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.drawEllipse(QPoint(0,0),scan_radius,scan_radius);
    //旋转扫描
    QConicalGradient conical_gradient(QPoint(0,0),scan_radius);
    conical_gradient.setAngle(-angleOffset*180.0);
    conical_gradient.setColorAt(0.0,Qt::green);
    conical_gradient.setColorAt(0.4,Qt::transparent);
    conical_gradient.setColorAt(1.0,Qt::transparent);
    painter.fillPath(ellipse_path,conical_gradient);
    /*painter.setBrush(conical_gradient);
    painter.setPen(Qt::NoPen);
    painter.drawPie(QRect(-scan_radius,-scan_radius,2*scan_radius,2*scan_radius),
                    -angleOffset*180.0*16,360*0.5*16);*/
    //加一条线
    painter.setPen(QPen(Qt::cyan,2));
    painter.drawLine(QPoint(0,0),
                     QPoint(scan_radius*cos(angleOffset*3.14159265),
                            scan_radius*sin(angleOffset*3.14159265)
                            ));
    painter.setRenderHint(QPainter::Antialiasing,false);
    //十字
    painter.setPen(QPen(Qt::white,1,Qt::DashLine));
    painter.drawLine(-scan_radius,0,scan_radius,0);
    painter.drawLine(0,-scan_radius,0,scan_radius);
    painter.restore();
}
