#include "MyCurve.h"

#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QtMath>
#include <QDebug>

MyCurve::MyCurve(QWidget *parent)
    : QWidget(parent)
{

}

void MyCurve::paintEvent(QPaintEvent *event)
{
    event->accept();
    QPainter painter(this);
    //先画一个白底黑框
    painter.fillRect(this->rect(),Qt::white);
    QPen pen(Qt::black);
    painter.setPen(pen);
    painter.drawRect(this->rect().adjusted(0,0,-1,-1)); //右下角会超出范围

    //设置
    painter.setRenderHint(QPainter::Antialiasing,true); //抗锯齿
    painter.setPen(QPen(Qt::black,1));

    const int line_count=5;
    const int item_height=height()/line_count;
    const int item_width=width()/10;
    QList<QPointF> point_list;
    for(int i=0;i<10;i++)
    {
        point_list.push_back(QPointF(item_width*(i+0.5),
                                     item_height/2+(i%2?1:-1)*item_height/2*(i/15.0+0.3)));
    }

    //1-直线连接
    QPainterPath path_1;
    path_1.moveTo(point_list.at(0));
    for(int i=1;i<point_list.count();i++){
        path_1.lineTo(point_list.at(i));
    }
    painter.drawPath(path_1);
    for(const QPointF &pt:point_list){
        painter.drawEllipse(pt,5,5); //画圆是为了便于观察是否过采样点
    }

    //2-二次贝塞尔
    painter.translate(0,item_height);
    QPainterPath path_2;
    path_2.moveTo(point_list.at(0));
    for(int i=1;i<point_list.count();i++){
        //控制点计算，x取两个点x中间，y取两者最大y，但由于绘制时y是反过来的，so取最小的
        const double ctrl_x=(point_list.at(i-1).x()+point_list.at(i).x())/2.0;
        const double ctrl_y=point_list.at(i-1).y()<point_list.at(i).y()
                ?point_list.at(i-1).y()
               :point_list.at(i).y();
        painter.drawEllipse(QPointF(ctrl_x,ctrl_y),3,3); //画出控制点
        path_2.quadTo(QPointF(ctrl_x,ctrl_y),point_list.at(i));
    }
    painter.drawPath(path_2);
    for(const QPointF &pt:point_list){
        painter.drawEllipse(pt,5,5); //画圆是为了便于观察是否过采样点
    }

    //3-三次贝塞尔
    painter.translate(0,item_height);
    QPainterPath path_3;
    path_3.moveTo(point_list.at(0));
    for(int i=1;i<point_list.count();i++){
        //控制点计算，x取两个点x中间，y分别取一次
        const double ctrl_x=(point_list.at(i-1).x()+point_list.at(i).x())/2.0;
        const double ctrl_y1=point_list.at(i-1).y();
        const double ctrl_y2=point_list.at(i).y();

        painter.drawEllipse(QPointF(ctrl_x,ctrl_y1),3,3); //画出控制点
        painter.drawEllipse(QPointF(ctrl_x,ctrl_y2),3,3); //画出控制点
        path_3.cubicTo(QPointF(ctrl_x,ctrl_y1),QPointF(ctrl_x,ctrl_y2),point_list.at(i));
    }
    painter.drawPath(path_3);
    for(const QPointF &pt:point_list){
        painter.drawEllipse(pt,5,5); //画圆是为了便于观察是否过采样点
    }

    //4-三次贝塞尔
    painter.translate(0,item_height);
    QPainterPath path_4;
    //待添加
    painter.drawPath(path_4);
    for(const QPointF &pt:point_list){
        painter.drawEllipse(pt,5,5); //画圆是为了便于观察是否过采样点
    }

    //5-自定义曲线
    painter.translate(0,item_height);
    QPainterPath path_5=generateSmoothCurve(point_list);
    painter.drawPath(path_5);
    for(const QPointF &pt:point_list){
        painter.drawEllipse(pt,5,5); //画圆是为了便于观察是否过采样点
    }
}

QPainterPath MyCurve::generateSmoothCurve(QList<QPointF> points, bool closed, double tension, int numberOfSegments)
{
    QList<double> ps;

    foreach (QPointF p, points) {
        ps << p.x() << p.y();
    }

    return MyCurve::generateSmoothCurve(ps, closed, tension, numberOfSegments);
}

QPainterPath MyCurve::generateSmoothCurve(QList<double> points, bool closed, double tension, int numberOfSegments)
{
    QList<double> ps(points); // clone array so we don't change the original points
    QList<double> result; // generated smooth curve coordinates
    double x, y;
    double t1x, t2x, t1y, t2y;
    double c1, c2, c3, c4;
    double st;

    // The algorithm require a previous and next point to the actual point array.
    // Check if we will draw closed or open curve.
    // If closed, copy end points to beginning and first points to end
    // If open, duplicate first points to befinning, end points to end
    if (closed) {
        ps.prepend(points[points.length() - 1]);
        ps.prepend(points[points.length() - 2]);
        ps.prepend(points[points.length() - 1]);
        ps.prepend(points[points.length() - 2]);
        ps.append(points[0]);
        ps.append(points[1]);
    } else {
        ps.prepend(points[1]); // copy 1st point and insert at beginning
        ps.prepend(points[0]);
        ps.append(points[points.length() - 2]); // copy last point and append
        ps.append(points[points.length() - 1]);
    }

    // 1. loop goes through point array
    // 2. loop goes through each segment between the 2 points + 1e point before and after
    for (int i = 2; i < (ps.length() - 4); i += 2) {
        // calculate tension vectors
        t1x = (ps[i + 2] - ps[i - 2]) * tension;
        t2x = (ps[i + 4] - ps[i - 0]) * tension;
        t1y = (ps[i + 3] - ps[i - 1]) * tension;
        t2y = (ps[i + 5] - ps[i + 1]) * tension;

        for (int t = 0; t <= numberOfSegments; t++) {
            // calculate step
            st = (double)t / (double)numberOfSegments;

            // calculate cardinals
            c1 = 2 * qPow(st, 3) - 3 * qPow(st, 2) + 1;
            c2 = -2 * qPow(st, 3) + 3 * qPow(st, 2);
            c3 = qPow(st, 3) - 2 * qPow(st, 2) + st;
            c4 = qPow(st, 3) - qPow(st, 2);

            // calculate x and y cords with common control vectors
            x = c1 * ps[i] + c2 * ps[i + 2] + c3 * t1x + c4 * t2x;
            y = c1 * ps[i + 1] + c2 * ps[i + 3] + c3 * t1y + c4 * t2y;

            //store points in array
            result << x << y;
        }
    }

    // 使用的平滑曲线的坐标创建 QPainterPath
    QPainterPath path;
    path.moveTo(result[0], result[1]);
    for (int i = 2; i < result.length() - 2; i += 2) {
        path.lineTo(result[i+0], result[i+1]);
    }

    if (closed) {
        path.closeSubpath();
    }

    return path;
}
