#include "XYView.h"
#include <QResizeEvent>
#include <QPaintEvent>
#include <QPainter>

XYView::XYView(QWidget *parent)
    : QWidget(parent)
{
    xAxis = new XYAxis(XYAxis::AtBottom,this);
    yAxis=new XYAxis(XYAxis::AtLeft,this);

    connect(xAxis,&XYAxis::axisChanged,this,&XYView::refresh);
    connect(yAxis,&XYAxis::axisChanged,this,&XYView::refresh);
}

void XYView::paintEvent(QPaintEvent *event)
{
event->accept();
QPainter painter(this);
painter.setPen(QColor(255,0,0,100));
xAxis->draw(&painter);
yAxis->draw(&painter);

painter.drawRect(rect().adjusted(0,0,-1,-1));
}

void XYView::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    QRect area =rect();
    xAxis->setRect(QRect(50,area.height()-50,area.width()-50 ,50));
    yAxis->setRect(QRect(0,0,50+1 ,area.height()-50+1));
}

void XYView::refresh()
{
    update();
}
