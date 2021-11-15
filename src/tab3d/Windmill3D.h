#pragma once
#include <QWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QVector3D>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QPainterPath>
#include <QSharedPointer>
#include <QTime>
#include <QTimer>
#include <QImage>
#include <QFutureWatcher>

//图元结构体
struct WindMeta
{
    //顶点，可以是任意个
    QList<QVector3D> vertex;
    //颜色
    QColor color;
    //QBrush brush;
    //图元顶点中z值最小者，单独作为成员便于排序
    double z;
    //根据定点计算出的路径，便于绘制
    QPainterPath path;

    //构造函数
    WindMeta(const QList<QVector3D> &vtx, const QColor &clr);
};

//物体实体结构体
struct WindItem
{
    //相对于场景或者父节点的坐标位置
    QVector3D position;
    //相对于场景或者父节点的方向
    QVector3D rotation;
    //包含的图元
    QList<QSharedPointer<WindMeta>> surfaceMetas;
    //子节点物体列表
    QList<QSharedPointer<WindItem>> subItems;
    //旋转动画因子(根据全局的定时器步进值计算对应分量动画效果)
    QVector3D animationFactor;

    //构造函数
    WindItem(const QVector3D &pos = QVector3D(0, 0, 0),
             const QVector3D &rotate = QVector3D(0, 0, 0),
             const QList<QSharedPointer<WindMeta>> &metas = QList<QSharedPointer<WindMeta>>(),
             const QList<QSharedPointer<WindItem>> &subs = QList<QSharedPointer<WindItem>>(),
             const QVector3D &factor = QVector3D(0, 0, 0));

    //根据当前位置和角度计算出顶点列表
    //position取出后直接叠加到顶点的坐标上:vertex+position+this->position
    //rotation目前只计算了x和y的旋转，作用于item的顶点上，目前只能旋转item
    //step为定时器动画的步进，每个item根据自身的动画因子成员来计算
    QList<QSharedPointer<WindMeta>> calcSurfaceMetas(
            const QVector3D &position, const QVector3D &rotation, double step);
};

//绘制一个3D风车
//（目前按大块平面来计算堆叠顺序效果不太好，两个物体交叉时会有一部分被覆盖）
class Windmill3D : public QWidget
{
    Q_OBJECT
public:
    explicit Windmill3D(QWidget *parent = nullptr);
    ~Windmill3D();

protected:
    //显示时才启动定时动画
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    //绘制
    void paintEvent(QPaintEvent *event) override;
    //鼠标操作
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    //改变窗口大小
    void resizeEvent(QResizeEvent *event) override;

private:
    //初始化操作
    void initWindmill();
    //绘制
    void drawImage(int width, int height);

private:
    //根实体（这个变量绘制时在线程访问）
    WindItem rootItem;
    //FPS统计，paintEvent累计temp，达到一秒后赋值给counter
    int fpsCounter{0};
    int fpsTemp{0};
    //FPS计时
    QTime fpsTime;

    //鼠标位置
    QPoint mousePos;
    //鼠标按下标志位
    bool mousePressed = false;

    //定时动画
    QTimer timer;
    //定时器旋转步进值
    double animationStep{0.0};
    //旋转角度
    double xRotate{0.0};
    double yRotate{0.0};

    //多线程异步watcher
    QFutureWatcher<QImage> watcher;
    //绘制好的image
    QImage image;
};
