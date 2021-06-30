#include "MyCurve.h"

#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QtMath>、
#include <QDateTime>
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
    const double item_height=height()/line_count;
    const double item_width=width()/10.0;
    QList<QPointF> point_list;
    //之所以手动填充点，是为了展示一些典型的曲线
    point_list.push_back(QPointF(item_width*0.5,item_height*0.95));
    point_list.push_back(QPointF(item_width*0.75,item_height*0.05));
    point_list.push_back(QPointF(item_width*1.0,item_height*0.5));
    point_list.push_back(QPointF(item_width*2.5,item_height*0.6));
    point_list.push_back(QPointF(item_width*3.5,item_height*0.95));
    point_list.push_back(QPointF(item_width*4.5,item_height*0.05));
    point_list.push_back(QPointF(item_width*5.5,item_height*0.95));
    point_list.push_back(QPointF(item_width*6.5,item_height*0.15));
    point_list.push_back(QPointF(item_width*7.5,item_height*0.5));
    //这里制造一个方形，目测三次贝塞尔地效果更好
    point_list.push_back(QPointF(item_width*7.51,item_height*0.8));
    point_list.push_back(QPointF(item_width*9.49,item_height*0.8));
    point_list.push_back(QPointF(item_width*9.5,item_height*0.15));

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
    painter.drawText(10,item_height/2,"折线 lineTo");

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
    painter.drawText(10,item_height/2,"二次贝塞尔 quadTo");

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
        //下面这个效果差不多，分别取1x2y
        //painter.drawEllipse(QPointF(point_list.at(i).x(),ctrl_y1),3,3); //画出控制点
        //painter.drawEllipse(QPointF(point_list.at(i-1).x(),ctrl_y2),3,3); //画出控制点
        path_3.cubicTo(QPointF(ctrl_x,ctrl_y1),QPointF(ctrl_x,ctrl_y2),point_list.at(i));
    }
    painter.drawPath(path_3);
    for(const QPointF &pt:point_list){
        painter.drawEllipse(pt,5,5); //画圆是为了便于观察是否过采样点
    }
    painter.drawText(10,item_height/2,"三次贝塞尔 cubicTo");

    //4-自定义曲线
    painter.translate(0,item_height);
    const QVector<QPointF> point_temp=point_list.toVector();
    const QVector<QPointF> points4 = point_temp;
    //使用时需要判断controlPoints4 size>=2
    const QVector<QPointF> controlPoints4 = calculateControlPoints(point_temp);
    QPainterPath path_4;
    path_4.moveTo(points4.at(0));
    for (int i = 0; i < points4.size() - 1; i++) {
        const QPointF &point = points4.at(i + 1);
        path_4.cubicTo(controlPoints4[2 * i], controlPoints4[2 * i + 1], point);
    }
    painter.drawPath(path_4);
    for(const QPointF &pt:point_list){
        painter.drawEllipse(pt,5,5); //画圆是为了便于观察是否过采样点
    }
    painter.drawText(10,item_height/2,"QtCharts SplineChartItem");

    //5-自定义曲线
    painter.translate(0,item_height);
    QPainterPath path_5=generateSmoothCurve(point_list);
    painter.drawPath(path_5);
    for(const QPointF &pt:point_list){
        painter.drawEllipse(pt,5,5); //画圆是为了便于观察是否过采样点
    }
    painter.drawText(10,item_height/2,"公孙二狗");
}

QVector<QPointF> MyCurve::calculateControlPoints(const QVector<QPointF> &points)
{
    //Calculates control points which are needed by QPainterPath.cubicTo function to draw the cubic Bezier cureve between two points.
    QVector<QPointF> controlPoints;
    controlPoints.resize(points.count() * 2 - 2);

    int n = points.count() - 1;

    if (n == 1) {
        //for n==1
        controlPoints[0].setX((2 * points[0].x() + points[1].x()) / 3);
        controlPoints[0].setY((2 * points[0].y() + points[1].y()) / 3);
        controlPoints[1].setX(2 * controlPoints[0].x() - points[0].x());
        controlPoints[1].setY(2 * controlPoints[0].y() - points[0].y());
        return controlPoints;
    }

    // Calculate first Bezier control points
    // Set of equations for P0 to Pn points.
    //
    //  |   2   1   0   0   ... 0   0   0   ... 0   0   0   |   |   P1_1    |   |   P0 + 2 * P1             |
    //  |   1   4   1   0   ... 0   0   0   ... 0   0   0   |   |   P1_2    |   |   4 * P1 + 2 * P2         |
    //  |   0   1   4   1   ... 0   0   0   ... 0   0   0   |   |   P1_3    |   |   4 * P2 + 2 * P3         |
    //  |   .   .   .   .   .   .   .   .   .   .   .   .   |   |   ...     |   |   ...                     |
    //  |   0   0   0   0   ... 1   4   1   ... 0   0   0   | * |   P1_i    | = |   4 * P(i-1) + 2 * Pi     |
    //  |   .   .   .   .   .   .   .   .   .   .   .   .   |   |   ...     |   |   ...                     |
    //  |   0   0   0   0   0   0   0   0   ... 1   4   1   |   |   P1_(n-1)|   |   4 * P(n-2) + 2 * P(n-1) |
    //  |   0   0   0   0   0   0   0   0   ... 0   2   7   |   |   P1_n    |   |   8 * P(n-1) + Pn         |
    //
    QVector<qreal> vector;
    vector.resize(n);

    vector[0] = points[0].x() + 2 * points[1].x();


    for (int i = 1; i < n - 1; ++i)
        vector[i] = 4 * points[i].x() + 2 * points[i + 1].x();

    vector[n - 1] = (8 * points[n - 1].x() + points[n].x()) / 2.0;

    QVector<qreal> xControl = firstControlPoints(vector);

    vector[0] = points[0].y() + 2 * points[1].y();

    for (int i = 1; i < n - 1; ++i)
        vector[i] = 4 * points[i].y() + 2 * points[i + 1].y();

    vector[n - 1] = (8 * points[n - 1].y() + points[n].y()) / 2.0;

    QVector<qreal> yControl = firstControlPoints(vector);

    for (int i = 0, j = 0; i < n; ++i, ++j) {

        controlPoints[j].setX(xControl[i]);
        controlPoints[j].setY(yControl[i]);

        j++;

        if (i < n - 1) {
            controlPoints[j].setX(2 * points[i + 1].x() - xControl[i + 1]);
            controlPoints[j].setY(2 * points[i + 1].y() - yControl[i + 1]);
        } else {
            controlPoints[j].setX((points[n].x() + xControl[n - 1]) / 2);
            controlPoints[j].setY((points[n].y() + yControl[n - 1]) / 2);
        }
    }
    return controlPoints;
}

QVector<qreal> MyCurve::firstControlPoints(const QVector<qreal> &vector)
{
    QVector<qreal> result;

    int count = vector.count();
    result.resize(count);
    result[0] = vector[0] / 2.0;

    QVector<qreal> temp;
    temp.resize(count);
    temp[0] = 0;

    qreal b = 2.0;

    for (int i = 1; i < count; i++) {
        temp[i] = 1 / b;
        b = (i < count - 1 ? 4.0 : 3.5) - temp[i];
        result[i] = (vector[i] - result[i - 1]) / b;
    }

    for (int i = 1; i < count; i++)
        result[count - i - 1] -= temp[count - i] * result[count - i];

    return result;
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
