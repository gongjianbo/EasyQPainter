#include "DrawBezier.h"

#include <QtMath>
#include <QPainter>
#include <QPaintEvent>
#include <QPainterPath>

DrawBezier::DrawBezier(QWidget *parent)
    : QWidget(parent)
{
    // 定时更新
    connect(&timer, &QTimer::timeout, this, [this]()
    {
        theProgress += 1;
        theProgress %= 100;
        update();
    });
}

void DrawBezier::showEvent(QShowEvent *event)
{
    timer.start(50);
    QWidget::showEvent(event);
}

void DrawBezier::hideEvent(QHideEvent *event)
{
    timer.stop();
    QWidget::hideEvent(event);
}

void DrawBezier::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    // 自适应半径
    QRect area = rect();
    int area_r = 200;
    int point_r = 4;
    // 绘制进度
    double progress = theProgress / 100.0;
    // 四个控制点绘制三阶贝塞尔
    QPoint p0(-area_r, area_r / 2);
    QPoint p1(-area_r, -area_r / 2);
    QPoint p2(area_r / 2, -area_r / 2);
    QPoint p3(area_r, area_r / 2);

    QPainter painter(this);
    painter.translate(area.center());
    painter.setRenderHint(QPainter::Antialiasing, true);
    // 连接控制点
    painter.setPen(QPen(Qt::black, 2));
    painter.drawLine(p0, p1);
    painter.drawLine(p1, p2);
    painter.drawLine(p2, p3);
    // 贝塞尔曲线
    QPainterPath bezier;
    bezier.moveTo(p0);
    bezier.cubicTo(p1, p2, p3);
    painter.setPen(QPen(Qt::red, 2));
    painter.drawPath(bezier);
    // 控制点
    painter.setPen(QPen(Qt::black, 2));
    painter.drawEllipse(p0, point_r, point_r);
    painter.drawEllipse(p1, point_r, point_r);
    painter.drawEllipse(p2, point_r, point_r);
    painter.drawEllipse(p3, point_r, point_r);

    // 每条线按相同的进度百分比取一个点，连接这些点
    QPoint p01 = (p1 - p0) * progress + p0;
    QPoint p12 = (p2 - p1) * progress + p1;
    QPoint p23 = (p3 - p2) * progress + p2;
    painter.setPen(QPen(Qt::green, 2));
    painter.drawLine(p01, p12);
    painter.drawLine(p12, p23);
    painter.drawEllipse(p01, point_r, point_r);
    painter.drawEllipse(p12, point_r, point_r);
    painter.drawEllipse(p23, point_r, point_r);

    //
    QPoint p012 = (p12 - p01) * progress + p01;
    QPoint p123 = (p23 - p12) * progress + p12;
    painter.setPen(QPen(Qt::blue, 2));
    painter.drawLine(p012, p123);
    painter.drawEllipse(p012, point_r, point_r);
    painter.drawEllipse(p123, point_r, point_r);

    // 直到只剩一个点，随着进度变化，这些点的集合构成的曲线就是贝塞尔曲线
    QPoint p0123 = (p123 - p012) * progress + p012;
    painter.setPen(QPen(Qt::red, 2));
    painter.drawEllipse(p0123, point_r, point_r);
}
