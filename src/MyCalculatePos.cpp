#include "MyCalculatePos.h"

#include <QTimer>
#include <QPainterPath>
#include <QtMath>
#include <QPainter>
#include <QPaintEvent>
#include <QDebug>

MyCalculatePos::MyCalculatePos(QWidget *parent)
    : QWidget(parent)
{
    //定时旋转
    //减小刷新间隔和步进角度可以让旋转更自然，但是更耗费cpu
    //可以注释掉Timer来练习绘制静止状态下的圆和线
    QTimer *timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,[=](){
        theRotate+=1;
        theRotate%=360;

        if(isHidden())
            return;
        update();
    });
    timer->start(100);

    initImg_1();
    initImg_2();
    initImg_3();
}

void MyCalculatePos::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
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
    //由内而外顺序计数，绘制时先画外面大的
    //第5层
    {
        painter.save();
        //自己计算，不用rotate，这样文字也是正的
        //painter.rotate(theRotate);
        draw_5(&painter,theRotate);
        painter.restore();
    }
    //第4层
    {
        painter.save();
        painter.rotate(-theRotate);
        draw_4(&painter);
        painter.restore();
    }
    //第3层
    {
        painter.save();
        painter.rotate(theRotate);
        painter.drawImage(-theImg_3.width()/2,
                          -theImg_3.height()/2,
                          theImg_3);
        painter.restore();
    }
    //第2层
    {
        painter.save();
        painter.rotate(-theRotate);
        painter.drawImage(-theImg_2.width()/2,
                          -theImg_2.height()/2,
                          theImg_2);
        painter.restore();
    }
    //第1层
    {
        painter.save();
        painter.rotate(theRotate);
        painter.drawImage(-theImg_1.width()/2,
                          -theImg_1.height()/2,
                          theImg_1);
        painter.restore();
    }
}

void MyCalculatePos::initImg_1()
{
    theImg_1=QImage(200,200,QImage::Format_ARGB32);
    theImg_1.fill(Qt::transparent);

    //画一个顺时针的圆盘
    QPainter painter(&theImg_1);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setRenderHint(QPainter::TextAntialiasing,true);
    painter.translate(theImg_1.width()/2,theImg_1.height()/2);//移动到中心点
    QFont font;
    font.setFamily("SimSun");
    font.setPixelSize(16);
    font.setBold(true);
    painter.setFont(font);

    const int radius=60;//最小半径
    QPainterPath path1;
    path1.addEllipse(QPoint(0,0),radius,radius);
    QPainterPath path2;
    const int text_height=painter.fontMetrics().height();
    path2.addEllipse(QPoint(0,0),radius+text_height+10,radius+text_height+10);
    painter.fillPath(path2-path1,QBrush(Qt::black));
    painter.fillPath(path1,QBrush(Qt::white));
    QList<QString> str_list{
        "乾","坎","艮","震","巽","离","坤","兑"
    };
    QList<QString> str_list2{
        "☰","☵","☶","☳","☴","☲","☷","☱"
    };
    const int step=360/str_list.count();//旋转步进
    for(int i=0;i<str_list.count();i++)
    {
        //默认屏幕坐标系上负下正
        painter.setPen(QPen(Qt::black,2));
        painter.drawLine(0,0,0,-radius/(i%2==0?2.0:3.0));
        //画内圈字符
        const QString text2=str_list2.at(i);
        painter.drawText(
                    -painter.fontMetrics().width(text2)/2,
                    (radius-25+text_height),
                    text2
                    );

        painter.setPen(QPen(Qt::white));
        //画外圈文字
        const QString text=str_list.at(i);
        //文字的起点在左下角
        painter.drawText(
                    -painter.fontMetrics().width(text)/2,
                    (radius+text_height),
                    text
                    );

        painter.rotate(step); //每次叠加旋转30度
    }
}

void MyCalculatePos::initImg_2()
{
    theImg_2=QImage(300,300,QImage::Format_ARGB32);
    theImg_2.fill(Qt::transparent);

    //画一个顺时针的圆盘
    QPainter painter(&theImg_2);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setRenderHint(QPainter::TextAntialiasing,true);
    painter.translate(theImg_2.width()/2,theImg_2.height()/2);//移动到中心点
    QFont font;
    font.setFamily("SimSun");
    font.setPixelSize(16);
    font.setBold(true);
    painter.setFont(font);

    const int radius=90;//最小半径
    QPainterPath path1;
    path1.addEllipse(QPoint(0,0),radius,radius);
    QPainterPath path2;
    const int text_height=painter.fontMetrics().height();
    path2.addEllipse(QPoint(0,0),radius+text_height+10,radius+text_height+10);
    painter.fillPath(path2-path1,QBrush(Qt::black));
    painter.setPen(QPen(Qt::white));
    QList<QString> str_list{
        "甲","乙","丙","丁","戊","己","庚","辛","壬","癸"
    };
    const int angle=360/str_list.count();
    for(int i=0;i<str_list.count();i++)
    {
        //默认屏幕坐标系上负下正，这里从原点下方开始绘制，这样文字的角度就反过来了
        const QString text=str_list.at(i);
        //文字的起点在左下角
        painter.drawText(
                    -painter.fontMetrics().width(text)/2,
                    (radius+text_height),
                    text
                    );
        painter.rotate(angle);
    }
}

void MyCalculatePos::initImg_3()
{
    theImg_3=QImage(300,300,QImage::Format_ARGB32);
    theImg_3.fill(Qt::transparent);

    //画一个顺时针的圆盘
    QPainter painter(&theImg_3);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setRenderHint(QPainter::TextAntialiasing,true);
    painter.translate(theImg_3.width()/2,theImg_3.height()/2);//移动到中心点
    QFont font;
    font.setFamily("SimSun");
    font.setPixelSize(16);
    font.setBold(true);
    painter.setFont(font);

    const int radius=120;//最小半径
    QPainterPath path1;
    path1.addEllipse(QPoint(0,0),radius,radius);
    QPainterPath path2;
    const int text_height=painter.fontMetrics().height();
    path2.addEllipse(QPoint(0,0),radius+text_height+10,radius+text_height+10);
    painter.fillPath(path2-path1,QBrush(Qt::black));
    painter.setPen(QPen(Qt::white));
    QList<QString> str_list{
        "子","丑","寅","卯","辰","巳","午","未","申","酉","戌","亥"
    };
    const int angle=360/str_list.count();
    for(int i=0;i<str_list.count();i++)
    {
        //默认屏幕坐标系上负下正，这里从原点下方开始绘制，这样文字的角度就反过来了
        const QString text=str_list.at(i);
        //文字的起点在左下角
        painter.drawText(
                    -painter.fontMetrics().width(text)/2,
                    (radius+text_height),
                    text
                    );
        painter.rotate(angle);
    }
}

void MyCalculatePos::draw_4(QPainter *painter)
{
    //画一个逆时针的表盘
    QFont font;
    font.setFamily("SimSun");
    font.setPixelSize(16);
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(QPen(Qt::black,2));

    const int radius=150;//最小半径
    const int step=6;//旋转步进
    painter->drawEllipse(QPoint(0,0),radius,radius);
    //可以看到旋转时文字会有抖动
    for(int i=0;i<360;i+=step)
    {
        if(i%30==0){
            //默认屏幕坐标系上负下正，这里y取反过来
            painter->drawLine(0,-radius,0,-(radius+15));
            const QString text=QString::number(i);
            //文字的起点在左下角
            painter->drawText(
                        -painter->fontMetrics().width(text)/2,
                        -(radius+20),
                        text
                        );
        }else{
            painter->drawLine(0,-radius,0,-(radius+5));
        }
        painter->rotate(-step); //逆时针旋转
    }
}

void MyCalculatePos::draw_5(QPainter *painter,int rotate)
{
    //画一个顺时针的表盘
    //用qpainter的rotate，文字也是旋转的，可以想文字是固定方向
    //要么绘制文字时反方向转一下，要么直接算坐标，不用rotate
    QFont font;
    font.setFamily("SimSun");
    font.setPixelSize(16);
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(QPen(Qt::black,2));

    const int radius=200;//最小半径
    const int step=6;//旋转步进
    const int text_height=painter->fontMetrics().height();
    painter->drawEllipse(QPoint(0,0),radius,radius);
    //可以看到旋转时文字会有抖动
    for(int i=0;i<=180;i+=step)
    {
        //旋转后坐标计算公式
        //x'=x*cos(a)-y*sin(a)
        //y'=x*sin(a)+y*cos(a)
        //角度转为弧度计算
        const double radians=qDegreesToRadians((double)(i+rotate));
        //以左侧为起点顺时针的话 (x:-radius,y:0)带入公式
        //即圆上一点坐标
        //x'=x0+R*cos(a)
        //y'=y0+R*sin(a)
        //本来算出来是逆时针，不过因为屏幕坐标系y轴反的，就成了顺时针
        //x或y取反都可以让表盘的数字顺序反一下，负负得正，但是左右上下颠倒了
        //本来xy不取反，初始是3点方向顺时针往9点方向，现在为9点顺时到3点
        const double x1=cos(radians)*radius;
        const double y1=sin(radians)*radius;
        if(i%30==0){
            const double x2=cos(radians)*(radius+15);
            const double y2=sin(radians)*(radius+15);
            //默认屏幕坐标系上负下正
            painter->drawLine(-x1,-y1,-x2,-y2);

            //文本的中心点也要计算
            const double x3=cos(radians)*(radius+30);
            const double y3=sin(radians)*(radius+30);
            const QString text=QString::number(i);
            const int text_width=painter->fontMetrics().width(text);
            //Qt文字绘制的起点在左下角，所以得到文本中心后，往左下偏移宽高的一半
            //上减下加，左减右加，这样相当于往x2y3左下角移动的，使文本中心点在计算的位置
            painter->drawText(
                        -x3-text_width/2,
                        -y3+text_height/2,
                        text
                        );
        }else{
            const double x2=cos(radians)*(radius+5);
            const double y2=sin(radians)*(radius+5);
            //默认屏幕坐标系上负下正
            painter->drawLine(-x1,-y1,-x2,-y2);
        }
    }
}
