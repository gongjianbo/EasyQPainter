#ifndef MYSIMPLE3D_H
#define MYSIMPLE3D_H

#include <QVector3D>
#include <QMatrix4x4>
#include <QtMath>
#include <QQuaternion>
#include <QPainterPath>
#include <QSharedPointer>
//
#include <QWidget>
#include <QTime>
#include <QImage>
#include <QFutureWatcher>

//图元结构体
struct My3DMeta
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

    My3DMeta(const QList<QVector3D> &vtx,const QColor &clr)
        :vertex(vtx),color(clr)
    {}
};

//物体实体结构体
struct My3DItem
{
    //相对于场景或者父节点的坐标位置
    QVector3D position;
    //相对于场景或者父节点的方向
    QVector3D rotation;
    //包含的图元
    QList<QSharedPointer<My3DMeta>> surfaceMetas;
    //子节点物体列表
    QList<QSharedPointer<My3DItem>> subItems;
    //旋转动画因子(根据全局的定时器步进值计算对应分量动画效果)
    QVector3D animationFactor;

    My3DItem(const QVector3D &pos=QVector3D(0,0,0),
             const QVector3D &rotate=QVector3D(0,0,0),
             const QList<QSharedPointer<My3DMeta>> &metas=QList<QSharedPointer<My3DMeta>>(),
             const QList<QSharedPointer<My3DItem>> &subs=QList<QSharedPointer<My3DItem>>(),
             const QVector3D &factor=QVector3D(0,0,0))
        :position(pos),rotation(rotate),surfaceMetas(metas),subItems(subs),animationFactor(factor)
    {}

    //根据当前位置和角度计算出顶点列表
    //position取出后直接叠加到顶点得坐标上:vertex+position+this->position
    //rotation目前只计算了x和y的旋转，作用于item的顶点上，毕竟写一个camera太复杂了，所以只能旋转item
    //step为定时器动画的步进，每个item根据自身的动画因子成员来计算
    QList<QSharedPointer<My3DMeta>> calculateSurfaceMetas(const QVector3D &position,const QVector3D &rotation,double step)
    {
        QVector3D cur_position=position+this->position;
        QVector3D cur_rotation=rotation+this->rotation;
        for(QSharedPointer<My3DMeta> meta:surfaceMetas)
        {
            QPainterPath path;
            double z;
            //【注意】我这里还没有去解决万向锁的问题，只有叶子节点才能进行z轴旋转
            //等把Qt这个QQuaternion类搞明白了再回来改
            QMatrix4x4 mat_anim;
            mat_anim.rotate(QQuaternion::fromEulerAngles(step*this->animationFactor));
            //先在正交投影里计算item旋转后的坐标，然后z+=1000，使z值不至于小于0（因为需要透视投影）
            //然后使用透视投影算出近大远小，放大50倍是因为整体太远了看不见
            QMatrix4x4 mat_rotate;
            mat_rotate.rotate(QQuaternion::fromEulerAngles(cur_rotation));
            QMatrix4x4 mat_per;
            mat_per.perspective(45.0f,1.0f,0.1f,2000.0f);
            mat_per.scale(50); //暂时固定50倍放大
            bool is_first=true;
            for(const QVector3D &vertex:meta->vertex)
            {
                QVector3D calc_rotate=(vertex*mat_anim+cur_position)*mat_rotate;
                //暂时固定z+=1000
                QVector3D calc_vertex=(calc_rotate+QVector3D(0,0,1000))*mat_per;
                //qDebug()<<calc_rotate<<calc_vertex;
                //第一个点单独处理
                if(is_first){
                    path.moveTo(calc_vertex.x(),calc_vertex.y());
                    z=calc_vertex.z();
                    is_first=false;
                }else{
                    path.lineTo(calc_vertex.x(),calc_vertex.y());
                    if(calc_vertex.z()<z){
                        z=calc_vertex.z();
                    }
                }
            }
            //qDebug()<<path<<z;
            meta->path=path;
            meta->z=z;
        }
        QList<QSharedPointer<My3DMeta>> surface_metas=surfaceMetas;
        for(QSharedPointer<My3DItem> item:subItems)
        {
            surface_metas+=item->calculateSurfaceMetas(cur_position,cur_rotation+step*this->animationFactor,step);
        }

        return surface_metas;
    }
};

//绘制一个3d风车
//目前还存在两个问题：
//1.堆叠顺序计算效果不大好，或许可以将表面裁减为多个小三角来解决；
//2.Qt的四元数类我还不会用，所以物体的相对旋转如果要计算z轴分量就有问题，目前只能叶子节点z轴旋转
class MySimple3D : public QWidget
{
    Q_OBJECT
public:
    explicit MySimple3D(QWidget *parent = nullptr);
    ~MySimple3D();

protected:
    void paintEvent(QPaintEvent *event) override;
    //鼠标操作
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    //改变窗口大小
    void resizeEvent(QResizeEvent *event) override;

private:
    //初始化操作
    void initItems();
    //绘制
    void drawImage(int width,int height);

private:
    //根实体（这个变量绘制时在线程访问）
    My3DItem rootItem;
    //FPS统计，paintEvent累计temp，达到一秒后赋值给counter
    int fpsCounter=0;
    int fpsTemp=0;
    //FPS计时
    QTime fpsTime;
    //鼠标位置
    QPoint mousePos;
    //鼠标按下标志位
    bool mousePressed=false;
    //旋转角度
    double xRotate=0;
    double yRotate=0;
    //多线程异步watcher
    QFutureWatcher<QImage> watcher;
    //绘制好的image
    QImage image;
    //定时器旋转步进值
    double animationStep=0;
};

#endif // MYSIMPLE3D_H
