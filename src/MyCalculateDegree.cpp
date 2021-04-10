#include "MyCalculateDegree.h"

#include <QTimer>
#include <QPainterPath>
#include <QtMath>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QDebug>

MyCalculateDegree::MyCalculateDegree(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true);

    //定时旋转
    QTimer *timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,[=](){
        theRotate+=2;
        theRotate%=360;

        if(isHidden())
            return;
        update();
    });
    timer->start(100);
}

void MyCalculateDegree::paintEvent(QPaintEvent *event)
{
    event->accept();
    QPainter painter(this);
    //先画一个白底黑框
    painter.fillRect(this->rect(),Qt::white);
    QPen pen(Qt::black);
    painter.setPen(pen);
    painter.drawRect(this->rect().adjusted(0,0,-1,-1)); //右下角会超出范围

    //抗锯齿
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setRenderHint(QPainter::TextAntialiasing,true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform,true);
    //移动坐标中心点到窗口中心，默认左上角为起点，往右下为正方向
    painter.translate(width()/2,height()/2);

    //画圆盘
    drawCircle(&painter);
    //画小球轨迹
    drawItemPath(&painter);
    //画夹角
    drawPosDegree(&painter);
}

void MyCalculateDegree::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();
    //以中心点来计算
    mousePos=event->pos()-rect().center();
    update();
}

void MyCalculateDegree::drawCircle(QPainter *painter)
{
    painter->save();

    QFont font;
    font.setFamily("SimSun");
    font.setPixelSize(16);
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(QPen(QColor(0,0,0),2));
    painter->setBrush(QColor(10,10,10));

    const int radius=220;//最小半径
    painter->drawEllipse(QPoint(0,0),radius,radius);
    const int step=6;//旋转步进
    const int text_height=painter->fontMetrics().height();
    //可以看到旋转时文字会有抖动
    for(int i=0;i<360;i+=step)
    {
        //旋转后坐标计算公式
        //x'=x*cos(a)-y*sin(a)
        //y'=x*sin(a)+y*cos(a)
        //角度转为弧度计算
        const double radians=qDegreesToRadians((double)(i));
        //以顶部为起点顺时针的话 (x:0,y:radius)带入公式
        const double x1=-sin(radians)*(radius+5);
        const double y1=cos(radians)*(radius+5);
        if(i%30==0){
            const double x2=-sin(radians)*(radius+15);
            const double y2=cos(radians)*(radius+15);
            //默认屏幕坐标系上负下正
            painter->drawLine(-x1,-y1,-x2,-y2);

            //文本的中心点也要计算
            const double x3=-sin(radians)*(radius+30);
            const double y3=cos(radians)*(radius+30);
            const QString text=QString::number(i);
            const int text_width=painter->fontMetrics().width(text);
            //文字的起点在左下角
            //上减下加，左减右加，这样相当于往x2y3左下角移动的，使文本中心点在计算的位置
            painter->drawText(
                        -x3-text_width/2,
                        -y3+text_height/2,
                        text
                        );
        }else{
            const double x2=-sin(radians)*(radius+10);
            const double y2=cos(radians)*(radius+10);
            //默认屏幕坐标系上负下正
            painter->drawLine(-x1,-y1,-x2,-y2);
        }
    }

    painter->restore();
}

void MyCalculateDegree::drawItemPath(QPainter *painter)
{
    painter->save();

    //红色虚线
    painter->setPen(QPen(QColor(200,0,0),2,Qt::DotLine));
    const int a=200;//长轴
    const int b=100;//短轴
    painter->rotate(-45); //椭圆没有角度参数
    painter->drawEllipse(QPoint(0,0),a,b);
    painter->rotate(45);

    //画物体小球
    painter->setPen(QPen(QColor(220,0,60),2));
    painter->setBrush(QColor(220,0,60,100));
    //这里先计算了标准椭圆下位置，再用旋转矩阵进行计算偏移后的
    //是按照角度旋转来的，如果需要根据轨迹匀速运动，需要计算，这里暂不需要
    //椭圆上点公式，A横长半轴，B竖短半轴
    //x'=x0+A*B*cos(a)/sqrt(pow(A*sin(a),2)+pow(B*cos(a),2))
    //y'=y0+A*B*sin(a)/sqrt(pow(A*sin(a),2)+pow(B*cos(a),2))
    //前面我们学过圆上一点坐标
    //x'=x0+R*cos(a)
    //y'=y0+R*sin(a)
    //椭圆也可以化为相应的表达，不过半径需要计算
    //R=A*B/sqrt(pow(A*sin(a),2)+pow(B*cos(a),2))
    const double radians1=qDegreesToRadians((double)(theRotate));
    const double r1=a*b/sqrt(pow(a*sin(radians1),2)+pow(b*cos(radians1),2));
    const double x1=r1*cos(radians1);
    const double y1=r1*sin(radians1);
    //旋转矩阵公式，旋转后坐标计算
    //x'=x*cos(a)-y*sin(a)
    //y'=x*sin(a)+y*cos(a)
    const double radians2=qDegreesToRadians(-(double)45.0); //注意角度和y取反了
    const double x2=x1*cos(radians2)-y1*sin(radians2);
    const double y2=y1*sin(radians2)+x1*cos(radians2);
    painter->drawLine(0,0,x2,-y2);
    painter->drawEllipse(QPointF(0,0),8,8);
    painter->drawEllipse(QPointF(x2,-y2),10,10);

    //画夹角
    painter->drawLine(0,0,0,-220);
    //画圆弧角度要*16
    //注意椭圆角度为逆时针45
    painter->drawArc(QRect(-50,-50,100,100),90*16,-(theRotate+45)%360*16);

    painter->restore();
}

void MyCalculateDegree::drawPosDegree(QPainter *painter)
{
    //判断点是否在圆范围内
    //if(pow(mousePos.x(),2)+pow(mousePos.y(),2)>pow(250,2))
    //hypot计算算术平方根
    if(std::hypot(mousePos.x(),mousePos.y())>250)
        return;

    painter->save();

    painter->setPen(QPen(QColor(255,215,0),2));
    //painter->drawLine(QPoint(0,0),mousePos);

    //从直角坐标转极坐标
    //r=根号(y^2+x^2)
    //角度=atan2(y,x) --atan2式已将象限纳入考量的反正切函数
    //或atan分段函数求角度
    //角度是以右侧为0点，顺时针半圆为正，逆时针半圆为负
    //const double arc_tan=qAtan2(pos.y()-center_pos.y(),pos.x()-center_pos.x());
    const double arc_tan=qAtan2(mousePos.y(),mousePos.x());
    const double x1=cos(arc_tan)*220;
    const double y1=sin(arc_tan)*220;
    painter->drawLine(0,0,x1,y1);

    //atan2结果是以右侧为0点，顺时针半圆为正，逆时针半圆为负，结果单位是弧度
    //此处需要转换为值正北为0点，顺时针增长，单位转为角度
    //注意这个顺时针是因为y在屏幕坐标系反得
    double arc_degree=arc_tan*180/M_PI;
    if(arc_degree<0){
        arc_degree+=360;
    }
    arc_degree-=270;
    if(arc_degree<0){
        arc_degree+=360;
    }
    //画圆弧角度要*16
    painter->drawArc(QRect(-60,-60,120,120),90*16,-(arc_degree)*16);
    QFont font;
    font.setFamily("SimSun");
    font.setPixelSize(20);
    font.setBold(true);
    painter->setFont(font);
    painter->drawText(100,100,QString::number(arc_degree,'f',2)+" °");

    painter->restore();
}
