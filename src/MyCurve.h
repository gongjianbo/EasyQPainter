#ifndef MYCURVE_H
#define MYCURVE_H

#include <QWidget>

//绘制曲线
class MyCurve : public QWidget
{
    Q_OBJECT
public:
    explicit MyCurve(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    //曲线函数来自公孙二狗：http://qtdebug.com/qt-smooth-curve-1/
    //重载函数
    QPainterPath generateSmoothCurve(QList<QPointF> points, bool closed = false, double tension = 0.5, int numberOfSegments = 16);

    /**
    * @brief 使用传入的曲线顶点坐标创建平滑曲线。
    *
    * @param points 曲线顶点坐标数组，
    * points[i+0] 是第 i 个点的 x 坐标，
    * points[i+1] 是第 i 个点的 y 坐标
    * @param closed 曲线是否封闭，默认不封闭
    * @param tension 密集程度，默认为 0.5
    * @param numberOfSegments 平滑曲线 2 个顶点间的线段数，默认为 16
    * @return 平滑曲线的 QPainterPath
    */
    QPainterPath generateSmoothCurve(QList<double>points, bool closed = false, double tension = 0.5, int numberOfSegments = 16);
};

#endif // MYCURVE_H
