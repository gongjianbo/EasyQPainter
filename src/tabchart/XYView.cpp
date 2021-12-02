#include "XYView.h"
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QDebug>

XYView::XYView(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    xAxis = new XYAxis(XYAxis::AtBottom,this);
    yAxis=new XYAxis(XYAxis::AtLeft,this);

    connect(xAxis,&XYAxis::axisChanged,this,&XYView::refresh);
    connect(yAxis,&XYAxis::axisChanged,this,&XYView::refresh);
}

void XYView::paintEvent(QPaintEvent *event)
{
    event->accept();
    QPainter painter(this);

    painter.setPen(QColor(0,180,200));
    auto x_tick=xAxis->getTickPos();
    auto y_tick=yAxis->getTickPos();
    for(int px : x_tick)
    {
        if(px>=plotArea.left()&& px<=plotArea.right()){
            painter.drawLine(px,plotArea.top(),px,plotArea.bottom());
        }
    }
    for(int px : y_tick)
    {
        if(px>=plotArea.top()&&px<=plotArea.bottom()){
            painter.drawLine(plotArea.left(),px,plotArea.right(),px);
        }
    }

    painter.setPen(QColor(255,0,0,100));
    xAxis->draw(&painter);
    yAxis->draw(&painter);
    painter.drawRect(rect().adjusted(10,10,-11,-11));

    if(plotArea.isValid() && plotArea.contains(mousePos))
    {
        painter.setPen(QColor(255,0,0));
        painter.drawLine(mousePos.x(),plotArea.top(),mousePos.x(),plotArea.bottom());
        painter.drawLine(plotArea.left(),mousePos.y(),plotArea.right(),mousePos.y());
        QPoint plot_pos = mousePos-plotArea.topLeft();
        QString val=QString("(%1,%2)")
                .arg(QString::number(xAxis->pxToValue(plot_pos.x()),'f',2))
                .arg(QString::number(yAxis->pxToValue(plotArea.height()-1-plot_pos.y()),'f',2));
        painter.drawText(mousePos+QPoint(10,-10),val);
    }
}

void XYView::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    int margin = 10;
    contentArea =rect().adjusted(margin,margin,-margin,-margin);
    xAxis->setRect(QRect(contentArea.left()+50,contentArea.bottom()-50,
                         contentArea.width()-50 ,50+1));
    yAxis->setRect(QRect(contentArea.left(),contentArea.top(),
                         50+1,contentArea.height()-50));

    plotArea = contentArea.adjusted(50,0,0,-50);
}

void XYView::mousePressEvent(QMouseEvent *event)
{
    event->accept();
    mousePos=event->pos();
    refresh();
}

void XYView::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();
    mousePos=event->pos();
    refresh();
}

void XYView::mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();
    mousePos=event->pos();
    refresh();
}

void XYView::refresh()
{
    update();
}
