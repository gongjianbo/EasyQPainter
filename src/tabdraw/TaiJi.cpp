#include "TaiJi.h"
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QGradient>
#include <QBrush>
#include <QTransform>

TaiJi::TaiJi(QWidget *parent)
    : QWidget(parent)
{
    connect(&timer, &QTimer::timeout, this, [this]
    {
        offset += 1;
        offset %= 360;
        update();
    });
}

void TaiJi::showEvent(QShowEvent *event)
{
    timer.start(50);
    QWidget::showEvent(event);
}

void TaiJi::hideEvent(QHideEvent *event)
{
    timer.stop();
    QWidget::hideEvent(event);
}

void TaiJi::paintEvent(QPaintEvent *event)
{
    event->accept();
    {
        QPainter painter(this);
        //黑色背景
        painter.fillRect(this->rect(), Qt::black);
    }
    taijiTest();
    taijiPaint();
}

void TaiJi::taijiTest()
{
    //此函数主要是验证旋转和图层遮挡的逻辑
    QPainter painter(this);
    //painter.fillRect(this->rect(), Qt::black);
    painter.setRenderHint(QPainter::Antialiasing);

    //两像素的画笔避免抗锯齿或者变换后框线看不清
    QPen pen;
    pen.setWidth(2);

    int radius = 150;
    //圆形，后面旋转z轴和y轴使两个平面相交
    QPainterPath ellipse_path;
    ellipse_path.addEllipse(QPointF(0, 0), radius, radius);

    //在圆形基础上加了十字线，用于观察旋转方向
    QPainterPath line_path = ellipse_path;
    line_path.moveTo(0, radius);
    line_path.lineTo(0, -radius);
    line_path.closeSubpath();
    line_path.moveTo(radius, 0);
    line_path.lineTo(-radius, 0);
    line_path.closeSubpath();

    //QTransform是一个二维变换类，可以和QPainter搭配使用
    QTransform trans;
    //中心移动到窗口中心偏左
    trans.translate(width() / 2 - radius - 10, height() / 2);
    //z轴旋转15度，效果是平面上右转了15度
    //后面的变换也是在此基础上，所以两个平面相交的部分是右倾的
    trans.rotate(15, Qt::ZAxis);
    //这里设置trans后，接下来的clip裁剪区域也是旋转了15度的
    painter.setTransform(trans);
    //在初次变换的基础上，z轴随时间偏移，产生旋转动画效果
    QTransform ztrans = trans;
    ztrans.rotate(offset, Qt::ZAxis);
    //开始画第一个面
    pen.setColor(Qt::red);
    painter.setPen(pen);
    {
        //save是为了clip不污染后面的操作
        painter.save();
        //clip顶部的矩形区域（底部是被另一个面遮盖的）
        painter.setClipRect(QRect(-radius, -radius, radius * 2, radius));
        painter.setTransform(ztrans);
        //填充这个区域会受clip的影响
        painter.fillPath(ellipse_path, QColor(255, 0, 0, 100));
        painter.restore();
    }
    //在没有clip的情况下绘制框线，以进行观察
    painter.setTransform(ztrans);
    painter.drawPath(line_path);

    //在z轴旋转15度的基础上，x轴再旋转50度，即顶部往里翻转了
    trans.rotate(50, Qt::XAxis);
    painter.setTransform(trans);
    //旋转的角度需要反过来
    QTransform xtrans = trans;
    xtrans.rotate(-offset, Qt::ZAxis);
    //开始画第二个面，逻辑同第一个面
    pen.setColor(Qt::cyan);
    painter.setPen(pen);
    {
        painter.save();
        painter.setClipRect(QRect(-radius, 0, radius * 2, radius));
        painter.setTransform(xtrans);
        painter.fillPath(ellipse_path, QColor(0, 255, 0, 100));
        painter.restore();
    }
    painter.setTransform(xtrans);
    painter.drawPath(line_path);
}

void TaiJi::taijiPaint()
{
    QPainter painter(this);
    //painter.fillRect(this->rect(), Qt::black);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen;
    //大圆半径
    int radius = 150;
    //小孔半径
    int sub_radius = radius / 5;
    //玉外面的球-圆心
    QPointF out_point(0, radius / 2);
    //玉小孔的圆心
    QPointF in_point(0, -radius / 2);
    //z轴旋转面的颜色
    QColor z_color(100, 100, 100);
    //x轴旋转面的颜色
    QColor x_color(200, 200, 200);

    //圆形，后面旋转z轴和y轴使两个平面相交
    QPainterPath ellipse_path;
    ellipse_path.addEllipse(QPointF(0, 0), radius, radius);

    //z轴旋转面的玉路径
    QPainterPath z_path;
    //奇偶填充，这样填充会把小孔留出空白
    z_path.setFillRule(Qt::OddEvenFill);
    z_path.moveTo(0, -radius);
    //一个大圆弧
    z_path.arcTo(QRectF(-radius, -radius, radius * 2, radius * 2), 90, 180);
    //两个小圆弧
    z_path.arcTo(QRectF(-radius / 2, 0, radius, radius), 270, -180);
    z_path.arcTo(QRectF(-radius / 2, -radius, radius, radius), 270, 180);
    //小孔
    z_path.addEllipse(in_point, sub_radius, sub_radius);
    z_path.closeSubpath();

    //x轴旋转面的玉路径，做两个是因为旋转方向相反，绘制取反后绘制的效果不大好
    QPainterPath x_path;
    x_path.setFillRule(Qt::OddEvenFill);
    x_path.moveTo(0, radius);
    x_path.arcTo(QRectF(-radius, -radius, radius * 2, radius * 2), 270, 180);
    x_path.arcTo(QRectF(-radius / 2, -radius, radius, radius), 90, 180);
    x_path.arcTo(QRectF(-radius / 2, 0, radius, radius), 90, -180);
    x_path.addEllipse(in_point, sub_radius, sub_radius);
    x_path.closeSubpath();

    //QTransform是一个二维变换类，可以和QPainter搭配使用
    QTransform trans;
    //中心移动到窗口中心偏右
    trans.translate(width() / 2 + radius + 10, height() / 2);
    //z轴旋转15度，效果是平面上右转了15度
    //后面的变换也是在此基础上，所以两个平面相交的部分是右倾的
    trans.rotate(15, Qt::ZAxis);
    //在初次变换的基础上，z轴随时间偏移，产生旋转动画效果
    QTransform ztrans = trans;
    ztrans.rotate(offset, Qt::ZAxis);
    //在z轴旋转15度的基础上，x轴再旋转50度，即顶部往里翻转了
    trans.rotate(50, Qt::XAxis);
    QTransform xtrans = trans;
    xtrans.rotate(-offset, Qt::ZAxis);

    //通过变换获取到小球和小孔圆心对应窗口实际的坐标
    QPointF z_out = ztrans.map(out_point);
    QPointF z_in = ztrans.map(in_point);
    QPainterPath z_ptpath;
    z_ptpath.addEllipse(z_out, sub_radius, sub_radius);
    QPointF x_out = xtrans.map(out_point);
    //QPointF x_in=xtrans.map(in_point);
    QPainterPath x_ptpath;
    x_ptpath.addEllipse(x_out, sub_radius, sub_radius);

    //两个小球的渐变填充，是看起来有点立体感
    QRadialGradient x_gradient(x_out, sub_radius);
    x_gradient.setColorAt(0, QColor(250, 250, 250));
    x_gradient.setColorAt(1, QColor(200, 200, 200));
    QRadialGradient z_gradient(z_out, sub_radius);
    z_gradient.setColorAt(0, QColor(150, 150, 150));
    z_gradient.setColorAt(1, QColor(100, 100, 100));

    //先绘制底层，即被遮盖的区域（相当于z值权重更低）
    {
        //绘制x轴旋转的小球,y小于另一个玉的小孔圆心y，表示当前被遮挡
        if (x_out.y() < z_in.y())
        {
            pen.setColor(Qt::red);
            painter.setPen(pen);
            painter.fillPath(x_ptpath, x_gradient);
        }

        painter.save();
        QTransform trans;
        trans.translate(width() / 2 + radius + 10, height() / 2);
        //z轴旋转15度，效果是平面上右转了15度
        trans.rotate(15, Qt::ZAxis);
        painter.setTransform(trans);
        {
            //save是为了clip不污染后面的操作
            painter.save();
            //clip顶部的矩形区域（底部是被另一个面遮盖的）
            painter.setClipRect(QRect(-radius, 0, radius * 2, radius));
            painter.setTransform(ztrans);
            painter.fillPath(z_path, z_color);
            painter.restore();
        }
        //在z轴旋转15度的基础上，x轴再旋转50度，即顶部往里翻转了
        trans.rotate(50, Qt::XAxis);
        painter.setTransform(trans);
        {
            //save是为了clip不污染后面的操作
            painter.save();
            //clip底部的矩形区域（顶部是被另一个面遮盖的）
            painter.setClipRect(QRect(-radius, -radius, radius * 2, radius));
            painter.setTransform(xtrans);
            painter.fillPath(x_path, x_color);
            painter.restore();
        }
        painter.restore();

        //绘制z轴旋转的小球
        pen.setColor(Qt::red);
        painter.setPen(pen);
        painter.fillPath(z_ptpath, z_gradient);
    }

    //绘制表层，逻辑同绘制底层
    {
        painter.save();
        QTransform trans;
        trans.translate(width() / 2 + radius + 10, height() / 2);
        trans.rotate(15, Qt::ZAxis);
        painter.setTransform(trans);
        {
            painter.save();
            //高度+2是为了遮盖两个平面相交部分clip+抗锯齿导致的虚线
            painter.setClipRect(QRect(-radius, -radius - 1, radius * 2, radius + 2));
            painter.setTransform(ztrans);
            painter.fillPath(z_path, z_color);
            painter.restore();
        }

        trans.rotate(50, Qt::XAxis);
        painter.setTransform(trans);
        {
            painter.save();
            painter.setClipRect(QRect(-radius, -1, radius * 2, radius + 2));
            painter.setTransform(xtrans);
            painter.fillPath(x_path, x_color);
            painter.restore();
        }
        painter.restore();

        //绘制x轴旋转的小球,y大于另一个玉的小孔圆心y，表示在表层
        if (x_out.y() >= z_in.y())
        {
            pen.setColor(Qt::red);
            painter.setPen(pen);
            painter.fillPath(x_ptpath, x_gradient);
        }
    }

    //小球定位测试
    //painter.drawEllipse(z_in,10,10);
    //painter.drawEllipse(z_out,10,10);
    //painter.drawEllipse(x_in,10,10);
    //painter.drawEllipse(x_out,10,10);
}
