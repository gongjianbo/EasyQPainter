#pragma once
#include <QWidget>

//表示饼图的一个数据项
struct PieSlice
{
    QString name; //名称
    double value; //值
    double percentage; //占总值的百分比
    double startAngle; //起始角度[0,360)
    double angleSpan; //占角度值[0,360)，为360*percentage
    QColor color; //颜色

    explicit PieSlice(const QString &name, double value = 0.0)
        : name(name), value(value) {
    }
};

//饼图绘制
class PieView : public QWidget
{
    Q_OBJECT
public:
    explicit PieView(QWidget *parent = nullptr);
    ~PieView();

    //添加数据项
    void appendSlice(const PieSlice &slice);
    //清除所有数据
    void clearSlice();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QList<PieSlice> sliceList; //slice数据容器
    double sliceValueCount{0.0}; //所有slice值之和

    //hover标志
    QPoint mousePos; //记录鼠标hover轨迹
    bool hoveredFlag; //是否hover
    int hoveredIndex; //当前选择的index

    //限定hover和绘制的范围
    int pieMargin{20};
    int pieRadius{20};
    QRectF pieRect; //饼图绘制区域，正方形居中
};
