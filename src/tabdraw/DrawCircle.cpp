#include "DrawCircle.h"

#include <QtMath>
#include <QPainter>
#include <QPaintEvent>

DrawCircle::DrawCircle(QWidget *parent)
    : QWidget(parent)
{
    // 定时旋转
    connect(&timer, &QTimer::timeout, this, [this]()
    {
        theRotate += 1;
        theRotate %= 360;
        update();
    });
}

void DrawCircle::showEvent(QShowEvent *event)
{
    timer.start(50);
    QWidget::showEvent(event);
}

void DrawCircle::hideEvent(QHideEvent *event)
{
    timer.stop();
    QWidget::hideEvent(event);
}

void DrawCircle::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    // 自适应半径
    QRect area = rect();
    int radius = area.width() > area.height() ? area.height() : area.width();
    radius = radius / 2 - 10;
    if (radius < 20) {
        radius = 20;
    }

    QPainter painter(this);
    painter.translate(area.center());
    painter.drawLine(0, radius, 0, -radius);
    painter.drawLine(radius, 0, -radius, 0);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawEllipse(QPoint(0, 0), radius / 2, radius / 2);

    // 这里是根据圆上点坐标倒推线条坐标
    // 根据直角三角形中线定理，三角形斜边上的中线等于斜边的一半
    // 所以线条坐标为点坐标数值的两倍
    const double a = qDegreesToRadians((double)theRotate);
    const int x = radius  * cos(a);
    const int y = radius  * sin(a);

    // 绘制线条和端点
    painter.setPen(QPen(Qt::red, 2));
    painter.drawLine(x, 0, 0, y);
    painter.drawEllipse(QPoint(x, 0), 5, 5);
    painter.drawEllipse(QPoint(0, y), 5, 5);

    // 绘制圆上点
    painter.setPen(QPen(Qt::green, 2));
    painter.drawEllipse(QPoint(x / 2, y / 2), 5, 5);
}
