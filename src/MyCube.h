#ifndef MYCUBE_H
#define MYCUBE_H

#include <QWidget>
#include <QMouseEvent>

#include <QVector3D>
#include <QMatrix4x4>

//绘制3d立方体
class MyCube : public QWidget
{
    Q_OBJECT
public:
    explicit MyCube(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    QPointF getPoint(const QVector3D &vt,int w) const;

private:
    QVector<QVector3D> vertexArr;     //八个顶点
    QVector<QVector<int>> elementArr; //六个面
    QMatrix4x4 rotateMat;      //旋转矩阵
    QPoint mousePos;           //鼠标位置
    bool mousePressed=false;   //鼠标按下标志位
};

#endif // MYCUBE_H
