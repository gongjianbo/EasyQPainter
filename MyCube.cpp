#include "MyCube.h"

#include <QPainter>
#include <QtMath>
#include <QTimer>
#include <QDebug>

MyCube::MyCube(QWidget *parent)
    : QWidget(parent)
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
    vertexArr=QVector<QVector3D>{
            QVector3D{1,1,1},
            QVector3D{1,-1,1},
            QVector3D{-1,-1,1},
            QVector3D{-1,1,1},

            QVector3D{1,1,-1},
            QVector3D{1,-1,-1},
            QVector3D{-1,-1,-1},
            QVector3D{-1,1,-1} };

    //六个面，一个面包含四个顶点
    elementArr=QVector<QVector<int>>{
    {0,1,2,3},
    {4,5,6,7},
    {0,4,5,1},
    {1,5,6,2},
    {2,6,7,3},
    {3,7,4,0} };

    //定时变化
    /*QTimer *timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,[=](){
        rotateMat.rotate(5,QVector3D(0.0f,0.0f,1.0f));
        update();
    });
    timer->start(150);*/

    setFocusPolicy(Qt::ClickFocus); //Widget默认没有焦点
}

void MyCube::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    //先画一个白底黑框
    painter.fillRect(this->rect(),Qt::white);
    QPen pen(Qt::black);
    painter.setPen(pen);
    painter.drawRect(this->rect().adjusted(0,0,-1,-1)); //右下角会超出范围

    //思路，找到z值最高的顶点，然后绘制该顶点相邻的面
    // 根据z值计算，近大远小
    //（此外，Qt是屏幕坐标系，原点在左上角）

    //计算顶点变换后坐标，包含zmax点就是正交表面可见的
    QList<QVector3D> vertex_list;
    QList<int> vertex_max_list; //top顶点在arr的位置
    float vertex_max_value; //top值
    //根据旋转矩阵计算每个顶点
    for(int i=0;i<vertexArr.count();i++)
    {
        QVector3D vertex=vertexArr.at(i)*rotateMat;
        vertex_list.push_back(vertex);
        if(i==0){
            vertex_max_list.push_back(0);
            vertex_max_value=vertex.z();
        }else{
            if(vertex.z()>vertex_max_value){
                vertex_max_list.clear();
                vertex_max_list.push_back(i);
                vertex_max_value=vertex.z();
            }else if(abs(vertex.z()-vertex_max_value)<(1E-7)){
                vertex_max_list.push_back(i);
            }
        }
    }

    //把原点移到中间来
    painter.save();
    painter.translate(width()/2,height()/2);
    //绘制front和back六个面，先计算路径再绘制
    QList<QPainterPath> element_path_list;
    QList<float> element_z_values; //每个面中心点的z值
    QList<QPointF> element_z_points;
    QList<int> element_front_list;
    const int cube_width=(width()>height()?height():width())/4;
    for(int i=0;i<elementArr.count();i++)
    {

        const QVector3D vt0=vertex_list.at(elementArr.at(i).at(0));
        const QVector3D vt1=vertex_list.at(elementArr.at(i).at(1));
        const QVector3D vt2=vertex_list.at(elementArr.at(i).at(2));
        const QVector3D vt3=vertex_list.at(elementArr.at(i).at(3));

        QPainterPath element_path;
        element_path.moveTo(getPoint(vt0,cube_width));
        element_path.lineTo(getPoint(vt1,cube_width));
        element_path.lineTo(getPoint(vt2,cube_width));
        element_path.lineTo(getPoint(vt3,cube_width));
        element_path.closeSubpath();

        //包含zmax点的就是正交表面可见的
        bool is_front=true;
        for(int vertex_index:vertex_max_list){
            if(!elementArr.at(i).contains(vertex_index)){
                is_front=false;
                break;
            }
        }
        if(is_front){
            element_front_list.push_back(i);
        }
        element_path_list.push_back(element_path);
        element_z_values.push_back((vt0.z()+vt2.z())/2);
        element_z_points.push_back((getPoint(vt0,cube_width)+getPoint(vt2,cube_width))/2);
    }

    //远小近大，还要把包含max但是被近大遮盖的去掉
    QList<int> element_front_remove;
    for(int i=0;i<element_front_list.count();i++)
    {
        for(int j=0;j<element_front_list.count();j++)
        {
            if(i==j)
                continue;
            const int index_i=element_front_list.at(i);
            const int index_j=element_front_list.at(j);
            if(element_z_values.at(index_i)>element_z_values.at(index_j)
                    &&element_path_list.at(index_i).contains(element_z_points.at(index_j))){
                element_front_remove.push_back(index_j);
            }
        }
    }
    for(int index:element_front_remove){
        element_front_list.removeOne(index);
    }

    //根据计算好的路径绘制
    painter.setRenderHint(QPainter::Antialiasing,true);
    for(auto index:element_front_list){
        painter.fillPath(element_path_list.at(index),Qt::green);
    }
    painter.setPen(QPen(Qt::white,1,Qt::DashLine));
    for(int i=0;i<element_path_list.count();i++){
        if(element_front_list.contains(i))
            continue;
        painter.drawPath(element_path_list.at(i));
    }
    painter.setPen(QPen(Qt::black,2));
    for(auto index:element_front_list){
        painter.drawPath(element_path_list.at(index));
    }
    painter.restore();

    painter.drawText(20,30,"Drag Moving");
}

void MyCube::mousePressEvent(QMouseEvent *event)
{
    mousePressed=true;
    mousePos=event->pos();
    QWidget::mousePressEvent(event);
}

void MyCube::mouseMoveEvent(QMouseEvent *event)
{
    if(mousePressed){
        const QPoint posOffset=event->pos()-mousePos;
        mousePos=event->pos();
        //旋转矩阵
        rotateMat.rotate(posOffset.x(),QVector3D(0.0f,-0.5f,0.0f));
        rotateMat.rotate(posOffset.y(),QVector3D(0.5f,0.0f,0.0f));
        update();
    }
    QWidget::mouseMoveEvent(event);
}

void MyCube::mouseReleaseEvent(QMouseEvent *event)
{
    mousePressed=false;
    QWidget::mouseReleaseEvent(event);
}

QPointF MyCube::getPoint(const QVector3D &vt,int w) const
{
    //z用来实现远小近大的计算
    const float z_offset=vt.z()*0.1;
    return QPointF{vt.x()*w*(1+z_offset),
                vt.y()*w*(1+z_offset)};
}
