#include "Cube3D.h"

#include <QPainter>
#include <QPainterPath>
#include <QtMath>
#include <QDebug>

Cube3D::Cube3D(QWidget *parent) : QWidget(parent)
{
    //          7------------------4
    //        /                 /  |
    //     3------------------0    |
    //     |                  |    |
    //     |                  |    |
    //     |                  |    |
    //     |                  |    |
    //     |    6             |    5
    //     |                  |  /
    //     2------------------1
    //立方体前后四个顶点，从右上角开始顺时针
    vertexArr = QVector<QVector3D>{
        QVector3D{1, 1, 1},
        QVector3D{1, -1, 1},
        QVector3D{-1, -1, 1},
        QVector3D{-1, 1, 1},

        QVector3D{1, 1, -1},
        QVector3D{1, -1, -1},
        QVector3D{-1, -1, -1},
        QVector3D{-1, 1, -1}};

    //六个面，一个面包含四个顶点
    elementArr = QVector<QVector<int>>{
        {0, 1, 2, 3},
        {4, 5, 6, 7},
        {0, 4, 5, 1},
        {1, 5, 6, 2},
        {2, 6, 7, 3},
        {3, 7, 4, 0}};

    //Widget默认没有焦点，此处设置为点击时获取焦点
    setFocusPolicy(Qt::ClickFocus);
}

void Cube3D::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    //先画一个白底黑框
    painter.fillRect(this->rect(), Qt::white);
    QPen pen(Qt::black);
    painter.setPen(pen);
    painter.drawRect(this->rect().adjusted(0, 0, -1, -1)); //右下角会超出范围

    //思路，找到z值最高的顶点，然后绘制该顶点相邻的面
    // 根据z值计算，近大远小
    //（此外，Qt是屏幕坐标系，原点在左上角）

    //矩形边框参考大小
    const int cube_width = (width() > height() ? height() : width()) / 4;

    //投影矩阵
    //(之前计算错误，向量放在了矩阵左侧，误打误撞也实现了效果)
    QMatrix4x4 perspective_mat;
    perspective_mat.perspective(projectionFovy, 1.0f, 0.1f, 100.0f);
    //观察矩阵
    QMatrix4x4 view_mat;
    view_mat.translate(0.0f, 0.0f, -5.0f);
    view_mat.rotate(rotationQuat);

    //计算顶点变换后坐标，包含z值max点就是正交表面可见的，
    //再计算下远小近大的透视投影效果齐活了
    QList<QVector3D> vertex_list; //和矩阵运算后的顶点
    QList<int> vertex_max_list;   //z最大值列表（z值可能重复），内容为vertexArr的下标
    float vertex_max_value;       //顶点列表z最大值
    //根据旋转矩阵计算每个顶点
    for (int i = 0; i < vertexArr.count(); i++)
    {
        //以物体中心为原点旋转
        QVector3D vertex = perspective_mat * view_mat * vertexArr.at(i);
        vertex.setZ(-vertex.z());
        vertex.setY(-vertex.y());
        vertex_list.push_back(vertex);
        //找出z值max的顶点
        if (i == 0)
        {
            vertex_max_list.push_back(0);
            vertex_max_value = vertex.z();
        }
        else
        {
            if (vertex.z() > vertex_max_value)
            {
                //找最大的z值
                vertex_max_list.clear();
                vertex_max_list.push_back(i);
                vertex_max_value = vertex.z();
            }
            else if (abs(vertex.z() - vertex_max_value) < (1E-7))
            {
                //和最大z值相等的也添加到列表
                vertex_max_list.push_back(i);
            }
        }
    }

    //把原点移到中间来，方便绘制
    painter.save();
    painter.translate(width() / 2, height() / 2);
    //绘制front和back六个面，先计算路径再绘制
    QList<QPainterPath> element_path_list; //每个面路径
    QList<float> element_z_values;         //每个面中心点的z值
    QList<QPointF> element_z_points;       //每个面中心点在平面对应xy值
    QList<int> element_front_list;         //elementArr中表面的index
    //计算每个表面
    for (int i = 0; i < elementArr.count(); i++)
    {
        //每个面四个顶点
        const QVector3D &vt0 = vertex_list.at(elementArr.at(i).at(0));
        const QVector3D &vt1 = vertex_list.at(elementArr.at(i).at(1));
        const QVector3D &vt2 = vertex_list.at(elementArr.at(i).at(2));
        const QVector3D &vt3 = vertex_list.at(elementArr.at(i).at(3));

        //单个面的路径，面根据大小等比放大
        QPainterPath element_path;
        element_path.moveTo(getPoint(vt0, cube_width));
        element_path.lineTo(getPoint(vt1, cube_width));
        element_path.lineTo(getPoint(vt2, cube_width));
        element_path.lineTo(getPoint(vt3, cube_width));
        element_path.closeSubpath();

        //包含zmax点的就是正交表面可见的
        bool is_front = true;
        for (int vertex_index : vertex_max_list)
        {
            if (!elementArr.at(i).contains(vertex_index))
            {
                is_front = false;
                break;
            }
        }
        if (is_front)
        {
            element_front_list.push_back(i);
        }
        element_path_list.push_back(element_path);
        //对角线中间点作为面的z
        element_z_values.push_back((vt0.z() + vt2.z()) / 2);
        //对角线中间点
        element_z_points.push_back((getPoint(vt0, cube_width) + getPoint(vt2, cube_width)) / 2);
    }

    //远小近大，还要把包含max但是被近大遮盖的去掉
    QList<int> element_front_remove;
    for (int i = 0; i < element_front_list.count(); i++)
    {
        for (int j = 0; j < element_front_list.count(); j++)
        {
            if (i == j)
                continue;
            const int index_i = element_front_list.at(i);
            const int index_j = element_front_list.at(j);
            if (element_z_values.at(index_i) > element_z_values.at(index_j) && element_path_list.at(index_i).contains(element_z_points.at(index_j)))
            {
                element_front_remove.push_back(index_j);
            }
        }
    }
    for (int index : element_front_remove)
    {
        element_front_list.removeOne(index);
    }

    //根据计算好的路径绘制
    painter.setRenderHint(QPainter::Antialiasing, true);
    //画表面
    for (auto index : element_front_list)
    {
        painter.fillPath(element_path_list.at(index), Qt::green);
    }
    //画被遮盖面的边框虚线
    painter.setPen(QPen(Qt::white, 1, Qt::DashLine));
    for (int i = 0; i < element_path_list.count(); i++)
    {
        if (element_front_list.contains(i))
            continue;
        painter.drawPath(element_path_list.at(i));
    }
    //画表面边框
    painter.setPen(QPen(Qt::black, 2));
    for (auto index : element_front_list)
    {
        painter.drawPath(element_path_list.at(index));
    }
    painter.restore();

    painter.drawText(20, 30, "Drag Moving");
}

void Cube3D::mousePressEvent(QMouseEvent *event)
{
    mousePressed = true;
    mousePos = event->pos();
    QWidget::mousePressEvent(event);
}

void Cube3D::mouseMoveEvent(QMouseEvent *event)
{
    if (mousePressed)
    {
        QVector2D diff = QVector2D(event->pos()) - QVector2D(mousePos);
        mousePos = event->pos();
        QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();
        rotationAxis = (rotationAxis + n).normalized();
        //不能对换乘的顺序
        rotationQuat = QQuaternion::fromAxisAndAngle(rotationAxis, 2.0f) * rotationQuat;

        update();
    }
    QWidget::mouseMoveEvent(event);
}

void Cube3D::mouseReleaseEvent(QMouseEvent *event)
{
    mousePressed = false;
    QWidget::mouseReleaseEvent(event);
}

void Cube3D::wheelEvent(QWheelEvent *event)
{
    event->accept();
#if (QT_VERSION <= QT_VERSION_CHECK(5, 15, 0))
    //const QPoint pos = event->pos();
    const int delta = event->delta();
#else
    //const QPoint pos = event->position().toPoint();
    const int delta = event->angleDelta().y();
#endif
    //fovy越小，模型看起来越大
    if (delta < 0)
    {
        //鼠标向下滑动为-，这里作为zoom out
        projectionFovy += 0.5f;
        if (projectionFovy > 90)
            projectionFovy = 90;
    }
    else
    {
        //鼠标向上滑动为+，这里作为zoom in
        projectionFovy -= 0.5f;
        if (projectionFovy < 1)
            projectionFovy = 1;
    }
    update();
}

QPointF Cube3D::getPoint(const QVector3D &vt, int w) const
{
    //可以用z来手动计算远小近大，也可以矩阵运算
    //const float z_offset=vt.z()*0.1;
    //return QPointF{ vt.x()*w*(1+z_offset), vt.y()*w*(1+z_offset) };
    return QPointF{vt.x() * w, vt.y() * w};
}
