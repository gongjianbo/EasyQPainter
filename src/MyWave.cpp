#include "MyWave.h"

#include <QPainter>
#include <QPainterPath>
#include <QTimer>
#include <QtMath>

MyWave::MyWave(QWidget *parent) : QWidget(parent)
{
    QTimer *timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,[=](){
        timeVal+=1;

        if(isHidden())
            return;
        update();
    });
    timer->start(100);
}

void MyWave::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    //波形曲线区域宽度
    const int series_width=500;
    //波形与圆间隔
    const int series_space=120;
    //波形幅度高度
    const int series_height=100;
    QList<int> r_list;
    //拖拽后move居中
    painter.translate((width()-(series_width+series_space*2))/2,
                      (height()-(series_height*6))/2);
    //前两个为普通的正弦波，最后一个为前面的幅度叠加
    {
        painter.save();
        //圆
        painter.translate(series_space,series_height);
        painter.setPen(QPen(Qt::black,2));
        const int the_r=20;
        r_list.append(the_r);
        //当前角度
        //timeVal当作时间因子，每隔一段时间跑一段弧度距离，转换为角度
        const float angle=timeVal*36.0/(2*the_r*M_PI);
        //painter.drawEllipse(QPoint(0,0),5,5);
        //画圆
        painter.drawEllipse(QPointF(0,0),the_r,the_r);
        painter.drawLine(-the_r,0,the_r,0);
        painter.drawLine(0,-the_r,0,the_r);
        //当前角度对应坐标点
        const float the_x=0+the_r*cos(-angle);
        const float the_y=0+the_r*sin(-angle);
        painter.setPen(QPen(Qt::red,2));
        painter.drawEllipse(QPointF(the_x,the_y),5,5);
        painter.drawLine(QPointF(the_x,the_y),QPointF(0,0));
        //时域波形
        painter.translate(series_space,0);
        painter.setPen(QPen(Qt::blue,2));
        QPainterPath the_series;
        the_series.moveTo(0,the_y);
        for(int i=0;i<series_width;i++)
        {
            the_series.lineTo(i,the_r*sin(i*3/float(the_r)-angle));
        }
        painter.drawPath(the_series);
        painter.setPen(QPen(Qt::black,2));
        painter.drawLine(0,0,series_width,0);
        painter.drawLine(0,-the_r,0,the_r);
        painter.setPen(QPen(Qt::red,2,Qt::DotLine));
        painter.drawLine(QPointF(the_x-series_space,the_y),QPointF(0,the_y));

        painter.restore();
    }

    {
        painter.save();
        //圆
        painter.translate(series_space,series_height*3);
        painter.setPen(QPen(Qt::black,2));
        const int the_r=60;
        r_list.append(the_r);
        //当前角度
        const float angle=timeVal*36.0/(2*the_r*M_PI);
        //painter.drawEllipse(QPoint(0,0),5,5);
        painter.drawEllipse(QPointF(0,0),the_r,the_r);
        painter.drawLine(-the_r,0,the_r,0);
        painter.drawLine(0,-the_r,0,the_r);
        const float the_x=0+the_r*cos(-angle);
        const float the_y=0+the_r*sin(-angle);
        painter.setPen(QPen(Qt::red,2));
        painter.drawEllipse(QPointF(the_x,the_y),5,5);
        painter.drawLine(QPointF(the_x,the_y),QPointF(0,0));
        //时域波形
        painter.translate(series_space,0);
        painter.setPen(QPen(Qt::blue,2));
        QPainterPath the_series;
        the_series.moveTo(0,the_y);
        for(int i=0;i<series_width;i++)
        {
            the_series.lineTo(i,the_r*sin(i*3/float(the_r)-angle));
        }
        painter.drawPath(the_series);
        painter.setPen(QPen(Qt::black,2));
        painter.drawLine(0,0,series_width,0);
        painter.drawLine(0,-the_r,0,the_r);
        painter.setPen(QPen(Qt::red,2,Qt::DotLine));
        painter.drawLine(QPointF(the_x-series_space,the_y),QPointF(0,the_y));

        painter.restore();
    }

    {
        painter.save();
        //圆
        painter.translate(series_space,series_height*5);
        painter.save();
        int r_temp=0;
        float x_temp=0;
        float y_temp=0;
        for(int i=r_list.count()-1;i>=0;i--)
        {
            const int the_r=r_list.at(i);
            r_temp+=the_r;
            //当前角度
            const float angle=timeVal*36.0/(2*the_r*M_PI);
            painter.setPen(QPen(Qt::black,2));
            painter.drawEllipse(QPointF(0,0),the_r,the_r);
            if(i==r_list.count()-1){
                painter.drawLine(-the_r,0,the_r,0);
                painter.drawLine(0,-the_r,0,the_r);
            }
            const float the_x=0+the_r*cos(-angle);
            const float the_y=0+the_r*sin(-angle);
            x_temp+=the_x;
            y_temp+=the_y;
            painter.setPen(QPen(Qt::red,2));
            painter.drawLine(QPointF(the_x,the_y),QPointF(0,0));
            painter.translate(the_x,the_y);
        }
        painter.restore();

        //时域波形
        painter.translate(series_space,0);
        painter.setPen(QPen(Qt::blue,2));
        QPainterPath the_series;
        the_series.moveTo(0,y_temp);
        for(int i=0;i<series_width;i++)
        {
            float y=0;
            for(int j=r_list.count()-1;j>=0;j--)
            {
                int the_r=r_list.at(j);
                float angle=timeVal*36.0/(2*the_r*M_PI);
                y+=the_r*sin(i*3/float(the_r)-angle) ;
            }
            the_series.lineTo(i,y);
        }
        painter.drawPath(the_series);
        painter.setPen(QPen(Qt::black,2));
        painter.drawLine(0,0,series_width,0);
        painter.drawLine(0,-r_temp,0,r_temp);
        painter.setPen(QPen(Qt::red,2,Qt::DotLine));
        painter.drawLine(QPointF(x_temp-series_space,y_temp),QPointF(0,y_temp));

        painter.restore();
    }
}
