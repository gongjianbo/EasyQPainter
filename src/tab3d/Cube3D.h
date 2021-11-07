#pragma once
#include <QWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QVector3D>
#include <QMatrix4x4>
#include <QQuaternion>

//绘制一个立方体盒子
class Cube3D : public QWidget
{
    Q_OBJECT
public:
    explicit Cube3D(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    QPointF getPoint(const QVector3D &vt, int w) const;

private:
    //立方体八个顶点
    QVector<QVector3D> vertexArr;
    //立方体六个面
    QVector<QVector<int>> elementArr;
    //观察矩阵旋转
    QVector3D rotationAxis;
    QQuaternion rotationQuat;
    //透视投影的fovy参数，视野范围
    float projectionFovy{30.0f};

    //鼠标位置
    QPoint mousePos;
    //鼠标按下标志位
    bool mousePressed{false};
};
