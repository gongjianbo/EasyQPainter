#pragma once
#include <QWidget>
#include <QMouseEvent>
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

    QPointF getPoint(const QVector3D &vt, int w) const;

private:
    //立方体八个顶点
    QVector<QVector3D> vertexArr;
    //立方体六个面
    QVector<QVector<int>> elementArr;
    //旋转矩阵
    QMatrix4x4 rotateMat;

    //鼠标位置
    QPoint mousePos;
    //鼠标按下标志位
    bool mousePressed{false};
};
