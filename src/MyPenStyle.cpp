#include "MyPenStyle.h"

#include <QPainter>
#include <QPainterPath>
#include <QTimer>

MyPenStyle::MyPenStyle(QWidget *parent)
    : QWidget(parent)
{
    //累计dash总长
    for(int item:customDash){
        dashCount+=item;
    }

    //定时移动虚线偏移，制作蚂蚁线效果
    QTimer *timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,[=](){
        ++dashOffset;
        dashOffset%=dashCount; //dash点和线一个区段的整体长度

        if(isHidden())
            return;
        update();
    });
    timer->start(150);
}

void MyPenStyle::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    //先画一个白底黑框
    painter.fillRect(this->rect(),Qt::white);
    QPen pen(Qt::black);
    painter.setPen(pen);
    painter.drawRect(this->rect().adjusted(0,0,-1,-1)); //右下角会超出范围

    //准备路径
    //item是元素可视大小,rect是元素整体整体占位大小
    const int rect_width=this->width()/3;//（3*2个圆）
    const int rect_height=this->height()/2;
    const int item_width=((this->width()/3<this->height()/2)
                          ?this->width()/3
                         :this->height()/2)-10;//取最短边为边长为宽度（3*2个圆）
    const int item_left=(rect_width-item_width)/2;//item在rect的位置
    const int item_top=(rect_height-item_width)/2;
    QPainterPath path;//(一个圆加一个方框)
    path.addRect(item_left,item_top,item_width,item_width);
    //path.addEllipse(item_left,item_top,item_width-10,item_width-10);
    path.addEllipse(QPoint(rect_width/2,rect_height/2),item_width/2-5,item_width/2-5);

    //依次试用pen style枚举值
    pen.setColor(Qt::darkBlue); //原谅色
    //（如果线宽是奇数，直线抗锯齿会模糊，这是Qt的bug）
    pen.setWidth(4); //宽度
    painter.setRenderHint(QPainter::Antialiasing); //不开抗锯齿曲线有锯齿
    painter.save(); //保存位置，等会儿换行用restore恢复

    //Qt::NoPen 啥都没有
    //Qt::SolidLine 实线，默认值
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);
    painter.drawPath(path);
    painter.drawText(item_left+10,item_top+20,"SolidLine");

    //Qt::DashLine 虚线
    pen.setStyle(Qt::DashLine);
    painter.setPen(pen);
    painter.translate(rect_width,0);//右移
    painter.drawPath(path);
    painter.drawText(item_left+10,item_top+20,"DashLine");

    //Qt::DotLine 点线
    pen.setStyle(Qt::DotLine);
    painter.setPen(pen);
    painter.translate(rect_width,0);//右移
    painter.drawPath(path);
    painter.drawText(item_left+10,item_top+20,"DotLine");

    //Qt::DashDotLine 混合
    painter.restore(); //恢复位置
    pen.setStyle(Qt::DashDotLine);
    painter.setPen(pen);
    painter.translate(0,rect_height);//下移
    painter.drawPath(path);
    painter.drawText(item_left+10,item_top+20,"DashDotLine");

    //Qt::DashDotDotLine 混合
    pen.setStyle(Qt::DashDotDotLine);
    painter.setPen(pen);
    painter.translate(rect_width,0);//右移
    painter.drawPath(path);
    painter.drawText(item_left+10,item_top+20,"DashDotDotLine");

    //Qt::CustomDashLine
    //除了要设置pen style为Qt::CustomDashLine外，
    //还需要调用setDashPattern来描述自定义虚线的样子
    pen.setStyle(Qt::CustomDashLine);
    //参数奇数为线长度，偶数为线间隔（绘制的时候他好像没把线宽考虑进去）
    pen.setDashPattern(customDash);
    pen.setDashOffset(dashOffset); //定时移动偏移，蚂蚁线效果
    painter.setPen(pen);
    painter.translate(rect_width,0);//右移
    painter.drawPath(path);
    painter.drawText(item_left+10,item_top+20,"CustomDashLine");
}
