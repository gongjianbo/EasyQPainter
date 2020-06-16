#include "MyDrawImage.h"

#include <QPainter>
#include <QPainterPath>

MyDrawImage::MyDrawImage(QWidget *parent)
    : QWidget(parent)
{
    imgA=QImage(":/img/xingkong.jpg");
    imgB=QImage(":/img/xiangrikui.jpg");
}

void MyDrawImage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    //先画一个黑底白框
    painter.fillRect(this->rect(),Qt::black);
    painter.setPen(QPen(Qt::white,1,Qt::DashLine));
    painter.drawRect(this->rect().adjusted(0,0,-1,-1)); //右下角会超出范围

    painter.drawImage(0,0,imgA);

}
