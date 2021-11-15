#include "Windmill3D.h"

#include <algorithm>
#include <cmath>

#include <QtMath>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QPainter>
#include <QPainterPath>
#include <QtConcurrent>
#include <QDebug>

WindMeta::WindMeta(const QList<QVector3D> &vtx, const QColor &clr)
    : vertex(vtx), color(clr)
{
}

WindItem::WindItem(const QVector3D &pos, const QVector3D &rotate,
                   const QList<QSharedPointer<WindMeta>> &metas,
                   const QList<QSharedPointer<WindItem>> &subs,
                   const QVector3D &factor)
    : position(pos), rotation(rotate), surfaceMetas(metas), subItems(subs), animationFactor(factor)
{
}

QList<QSharedPointer<WindMeta>> WindItem::calcSurfaceMetas(
        const QVector3D &position, const QVector3D &rotation, double step)
{
    QVector3D cur_position = position + this->position;
    QVector3D cur_rotation = rotation + this->rotation;
    for (QSharedPointer<WindMeta> meta : surfaceMetas)
    {
        QPainterPath path;
        double z;
        //【注意】我这里还没有去解决万向锁的问题，只有叶子节点才能进行z轴旋转
        //等把Qt这个QQuaternion类搞明白了再回来改
        QMatrix4x4 mat_anim;
        mat_anim.rotate(QQuaternion::fromEulerAngles(step * this->animationFactor));
        //先在正交投影里计算item旋转后的坐标，然后z+=1000，使z值不至于小于0（因为需要透视投影）
        //然后使用透视投影算出近大远小，放大50倍是因为整体太远了看不见
        QMatrix4x4 mat_rotate;
        mat_rotate.rotate(QQuaternion::fromEulerAngles(cur_rotation));
        QMatrix4x4 mat_per;
        mat_per.perspective(45.0f, 1.0f, 0.1f, 2000.0f);
        mat_per.scale(50); //暂时固定50倍放大
        bool is_first = true;
        for (const QVector3D &vertex : meta->vertex)
        {
            QVector3D calc_rotate = (vertex * mat_anim + cur_position) * mat_rotate;
            //暂时固定z+=1000
            QVector3D calc_vertex = (calc_rotate + QVector3D(0, 0, 1000)) * mat_per;
            //qDebug()<<calc_rotate<<calc_vertex;
            //第一个点单独处理
            if (is_first)
            {
                path.moveTo(calc_vertex.x(), calc_vertex.y());
                z = calc_vertex.z();
                is_first = false;
            }
            else
            {
                path.lineTo(calc_vertex.x(), calc_vertex.y());
                if (calc_vertex.z() < z)
                {
                    z = calc_vertex.z();
                }
            }
        }
        //qDebug()<<path<<z;
        meta->path = path;
        meta->z = z;
    }
    QList<QSharedPointer<WindMeta>> surface_metas = surfaceMetas;
    for (QSharedPointer<WindItem> item : subItems)
    {
        surface_metas += item->calcSurfaceMetas(cur_position, cur_rotation + step * this->animationFactor, step);
    }

    return surface_metas;
}

Windmill3D::Windmill3D(QWidget *parent)
    : QWidget(parent)
{
    initWindmill();
    //异步处理结束，获取结果并刷新窗口
    connect(&watcher, &QFutureWatcher<QImage>::finished, [this]()
    {
        image = watcher.result();
        update();
    });

    fpsTime = QTime::currentTime();
    fpsTime.start();

    //定时旋转风车
    connect(&timer, &QTimer::timeout, this, [this]()
    {
        animationStep += 2.0;
        drawImage(width(), height());
    });
}

Windmill3D::~Windmill3D()
{
    if (!watcher.isFinished())
        watcher.waitForFinished();
}

void Windmill3D::showEvent(QShowEvent *event)
{
    timer.start(50);
    QWidget::showEvent(event);
}

void Windmill3D::hideEvent(QHideEvent *event)
{
    timer.stop();
    QWidget::hideEvent(event);
}

void Windmill3D::paintEvent(QPaintEvent *event)
{
    event->accept();
    QPainter painter(this);
    painter.fillRect(this->rect(), Qt::black);

    if (image.size().isValid())
        painter.drawImage(0, 0, image);

    //fps统计
    if (fpsTime.elapsed() > 1000)
    {
        fpsTime.restart();
        fpsCounter = fpsTemp;
        fpsTemp = 0;
    }
    else
    {
        fpsTemp++;
    }
    painter.setPen(QPen(Qt::white));
    painter.drawText(10, 30, "FPS:" + QString::number(fpsCounter));
    painter.drawText(10, 50, "Drag Moving ... ...");
}

void Windmill3D::mousePressEvent(QMouseEvent *event)
{
    mousePressed = true;
    mousePos = event->pos();
    QWidget::mousePressEvent(event);
}

void Windmill3D::mouseMoveEvent(QMouseEvent *event)
{
    if (mousePressed)
    {
        const QPoint posOffset = event->pos() - mousePos;
        mousePos = event->pos();
        //旋转矩阵  x和y分量
        xRotate += -posOffset.y();
        yRotate += -posOffset.x();
        //update();
        drawImage(width(), height());
    }
    QWidget::mouseMoveEvent(event);
}

void Windmill3D::mouseReleaseEvent(QMouseEvent *event)
{
    mousePressed = false;
    QWidget::mouseReleaseEvent(event);
}

void Windmill3D::resizeEvent(QResizeEvent *event)
{
    if (event->size().isValid())
    {
        const int width = event->size().width();
        const int height = event->size().height();
        drawImage(width, height);
    }
    QWidget::resizeEvent(event);
}

void Windmill3D::initWindmill()
{
    //模板的嵌套时自动格式化太难看了
    //四个扇叶
    WindMeta *sub_fan1 = new WindMeta{{QVector3D(0, 0, 0), QVector3D(-250, 250, 0),
            QVector3D(-300, 200, 10), QVector3D(-100, 0, 10)},
            QColor(110, 250, 250, 200)};
    WindMeta *sub_fan2 = new WindMeta{{QVector3D(0, 0, 0), QVector3D(-250, -250, 0),
            QVector3D(-200, -300, 10), QVector3D(0, -100, 10)},
            QColor(130, 250, 250, 200)};
    WindMeta *sub_fan3 = new WindMeta{{QVector3D(0, 0, 0), QVector3D(250, -250, 0),
            QVector3D(300, -200, 10), QVector3D(100, 0, 10)},
            QColor(110, 250, 250, 200)};
    WindMeta *sub_fan4 = new WindMeta{{QVector3D(0, 0, 0), QVector3D(250, 250, 0),
            QVector3D(200, 300, 10), QVector3D(0, 100, 10)},
            QColor(130, 250, 250, 200)};
    auto sub_fanmetas = QList<QSharedPointer<WindMeta>>{QSharedPointer<WindMeta>(sub_fan1),
                                                       QSharedPointer<WindMeta>(sub_fan2),
                                                       QSharedPointer<WindMeta>(sub_fan3),
                                                       QSharedPointer<WindMeta>(sub_fan4)};
    auto sub_fansubs = QList<QSharedPointer<WindItem>>{};
    WindItem *sub_fanitem = new WindItem{
            QVector3D(0, 400, -150),
            QVector3D(0, 0, 0),
            sub_fanmetas,
            sub_fansubs,
            QVector3D(0, 0, -1)}; //给z加了动画因子

    //风车主干，共9个面，顶部尖塔4+主干4+底面
    //顶部4
    WindMeta *sub_main1 = new WindMeta{{QVector3D(100, 400, 100), QVector3D(-100, 400, 100), QVector3D(0, 500, 0)},
            QColor(250, 0, 0)};
    WindMeta *sub_main2 = new WindMeta{{QVector3D(-100, 400, 100), QVector3D(-100, 400, -100), QVector3D(0, 500, 0)},
            QColor(0, 250, 0)};
    WindMeta *sub_main3 = new WindMeta{{QVector3D(-100, 400, -100), QVector3D(100, 400, -100), QVector3D(0, 500, 0)},
            QColor(0, 0, 250)};
    WindMeta *sub_main4 = new WindMeta{{QVector3D(100, 400, -100), QVector3D(100, 400, 100), QVector3D(0, 500, 0)},
            QColor(250, 250, 0)};
    //主体4
    WindMeta *sub_main5 = new WindMeta{{QVector3D(100, 400, 100), QVector3D(-100, 400, 100),
            QVector3D(-120, 0, 120), QVector3D(120, 0, 120)},
            QColor(205, 150, 100)};
    WindMeta *sub_main6 = new WindMeta{{QVector3D(-100, 400, 100), QVector3D(-100, 400, -100),
            QVector3D(-120, 0, -120), QVector3D(-120, 0, 120)},
            QColor(220, 150, 100)};
    WindMeta *sub_main7 = new WindMeta{{QVector3D(-100, 400, -100), QVector3D(100, 400, -100),
            QVector3D(120, 0, -120), QVector3D(-120, 0, -120)},
            QColor(235, 150, 100)};
    WindMeta *sub_main8 = new WindMeta{{QVector3D(100, 400, -100), QVector3D(100, 400, 100),
            QVector3D(120, 0, 120), QVector3D(120, 0, -120)},
            QColor(250, 150, 100)};
    //底部1
    WindMeta *sub_main9 = new WindMeta{{QVector3D(-120, 0, 120), QVector3D(-120, 0, -120),
            QVector3D(120, 0, -120), QVector3D(120, 0, 120)},
            QColor(200, 150, 0)};

    auto sub_mainmetas = QList<QSharedPointer<WindMeta>>{QSharedPointer<WindMeta>(sub_main1),
                                                        QSharedPointer<WindMeta>(sub_main2),
                                                        QSharedPointer<WindMeta>(sub_main3),
                                                        QSharedPointer<WindMeta>(sub_main4),
                                                        QSharedPointer<WindMeta>(sub_main5),
                                                        QSharedPointer<WindMeta>(sub_main6),
                                                        QSharedPointer<WindMeta>(sub_main7),
                                                        QSharedPointer<WindMeta>(sub_main8),
                                                        QSharedPointer<WindMeta>(sub_main9)};
    auto sub_mainsubs = QList<QSharedPointer<WindItem>>{QSharedPointer<WindItem>(sub_fanitem)};
    WindItem *sub_mainitem = new WindItem{
            QVector3D(0, 0, 0),
            QVector3D(0, 0, 0),
            sub_mainmetas,
            sub_mainsubs};

    //根节点，一个平面，（平面用半透明是为了穿模时看起来没那么别扭）
    WindMeta *root_meta = new WindMeta{{QVector3D(-200, 0, 200), QVector3D(200, 0, 200),
            QVector3D(200, 0, -200), QVector3D(-200, 0, -200)},
            QColor(255, 255, 255, 100)};
    auto root_metas = QList<QSharedPointer<WindMeta>>{QSharedPointer<WindMeta>(root_meta)};
    auto root_subs = QList<QSharedPointer<WindItem>>{QSharedPointer<WindItem>(sub_mainitem)};
    rootItem = WindItem{
            QVector3D(0, -300, 0),
            QVector3D(0, 0, 0),
            root_metas,
            root_subs,
            QVector3D(0, 0.1f, 0)}; //给y加了动画因子
}

void Windmill3D::drawImage(int width, int height)
{
    if (width > 10 && height > 10 && watcher.isFinished())
    {
        QVector3D rotate = QVector3D(xRotate, yRotate, 0);
        int step = animationStep;

        //多线程绘制到image上，绘制完后返回image并绘制到窗口上
        QFuture<QImage> futures =
                QtConcurrent::run([this, width, height, rotate, step]()
        {
            QImage img(width, height, QImage::Format_ARGB32);
            img.fill(Qt::transparent);
            QPainter painter(&img);
            if (!painter.isActive())
                return img;
            painter.fillRect(img.rect(), Qt::black);

            //painter.save();
            //坐标原点移动到中心
            painter.translate(width / 2, height / 2);

            //计算所有的图元顶点路径
            QList<QSharedPointer<WindMeta>> surface_metas = rootItem.calcSurfaceMetas(QVector3D(0, 0, 0), rotate, step);
            //根据z轴排序
            std::sort(surface_metas.begin(), surface_metas.end(),
                      [](const QSharedPointer<WindMeta> &left, const QSharedPointer<WindMeta> &right)
            {
                return left->z < right->z;
            });
            //根据z值从远处开始绘制图元路径
            for (QSharedPointer<WindMeta> meta : surface_metas)
            {
                painter.fillPath(meta->path, meta->color);
            }

            //painter.restore();
            return img;
        });
        watcher.setFuture(futures);
    }
}
