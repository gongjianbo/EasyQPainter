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
    : position(pos)
    , rotation(rotate)
    , surfaceMetas(metas)
    , subItems(subs)
    , animationFactor(factor)
{
}

QList<QSharedPointer<WindMeta>>
WindItem::calcSurfaceMetas(const QVector3D &position, const QQuaternion &rotation, float step, float fovy)
{
    QVector3D cur_position = position + this->position;
    //这里没验证，因为目前只为0，可能有误
    QQuaternion cur_rotation = QQuaternion::fromEulerAngles(this->rotation) * rotation;
    //平移做裁剪，缩放拉近距离
    QMatrix4x4 perspective_mat;
    perspective_mat.scale(100);
    perspective_mat.perspective(fovy, 1.0f, 0.1f, 2000.0f);
    QMatrix4x4 view_mat;
    view_mat.translate(0.0f, 0.0f, -1000.0f);
    //先跟随父节点转动和位移，再以自身的转动步进进行转动
    QMatrix4x4 model_mat;
    model_mat.rotate(cur_rotation);
    model_mat.translate(cur_position);
    model_mat.rotate(QQuaternion::fromEulerAngles(step * this->animationFactor));
    for (QSharedPointer<WindMeta> meta : qAsConst(surfaceMetas))
    {
        QPainterPath path;
        double z;
        bool is_first = true;
        for (const QVector3D &vertex : qAsConst(meta->vertex))
        {
            QVector3D calc_vertex= perspective_mat * view_mat * model_mat * vertex;
            calc_vertex.setY(-calc_vertex.y());
            calc_vertex.setZ(-calc_vertex.z());
            //qDebug()<<calc_vertex;
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
    cur_rotation *= QQuaternion::fromEulerAngles(step * this->animationFactor);
    QList<QSharedPointer<WindMeta>> surface_metas = surfaceMetas;
    for (QSharedPointer<WindItem> item : qAsConst(subItems))
    {
        surface_metas += item->calcSurfaceMetas(cur_position, cur_rotation, step, fovy);
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

    //之前使用的QTime，现在替换为QElapsedTimer
    //QTime fpsTime = QTime::currentTime();
    fpsTime.start();

    //定时旋转风车
    connect(&timer, &QTimer::timeout, this, [this]()
    {
        animationStep += 2.0f;
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
        QVector2D diff = QVector2D(event->pos()) - QVector2D(mousePos);
        mousePos = event->pos();
        QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();
        rotationAxis = (rotationAxis + n).normalized();
        //不能对换乘的顺序
        rotationQuat = QQuaternion::fromAxisAndAngle(rotationAxis, 2.0f) * rotationQuat;

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

void Windmill3D::wheelEvent(QWheelEvent *event)
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
    //update();
    drawImage(width(), height());
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
    //参照荷兰风车，逆时针旋转，帆布往塔身一侧倾斜
    //四个扇叶
    WindMeta *sub_fan1 = new WindMeta{{QVector3D(0, 0, 0), QVector3D(-250, -250, 0),
            QVector3D(-300, -200, -10), QVector3D(-100, 0, -10)},
            QColor(110, 250, 250, 200)};
    WindMeta *sub_fan2 = new WindMeta{{QVector3D(0, 0, 0), QVector3D(-250, 250, 0),
            QVector3D(-200, 300, -10), QVector3D(0, 100, -10)},
            QColor(130, 250, 250, 200)};
    WindMeta *sub_fan3 = new WindMeta{{QVector3D(0, 0, 0), QVector3D(250, 250, 0),
            QVector3D(300, 200, -10), QVector3D(100, 0, -10)},
            QColor(110, 250, 250, 200)};
    WindMeta *sub_fan4 = new WindMeta{{QVector3D(0, 0, 0), QVector3D(250, -250, 0),
            QVector3D(200, -300, -10), QVector3D(0, -100, -10)},
            QColor(130, 250, 250, 200)};
    auto sub_fanmetas = QList<QSharedPointer<WindMeta>>{QSharedPointer<WindMeta>(sub_fan1),
                                                       QSharedPointer<WindMeta>(sub_fan2),
                                                       QSharedPointer<WindMeta>(sub_fan3),
                                                       QSharedPointer<WindMeta>(sub_fan4)};
    auto sub_fansubs = QList<QSharedPointer<WindItem>>{};
    WindItem *sub_fanitem = new WindItem{
            QVector3D(0, 400, 150), //相对位置，y400放到顶部，z150贴在墙上
            QVector3D(0, 0, 0), //相对方向
            sub_fanmetas,
            sub_fansubs,
            QVector3D(0, 0, 1)}; //给z加了动画因子，即扇叶在xy平面转

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
            QVector3D(0, 0, 0), //相对位置
            QVector3D(0, 0, 0), //相对方向
            sub_mainmetas,
            sub_mainsubs};

    //根节点，一个平面，（平面用半透明是为了穿模时看起来没那么别扭）
    WindMeta *root_meta = new WindMeta{{QVector3D(-200, 0, 200), QVector3D(200, 0, 200),
            QVector3D(200, 0, -200), QVector3D(-200, 0, -200)},
            QColor(255, 255, 255, 100)};
    auto root_metas = QList<QSharedPointer<WindMeta>>{QSharedPointer<WindMeta>(root_meta)};
    auto root_subs = QList<QSharedPointer<WindItem>>{QSharedPointer<WindItem>(sub_mainitem)};
    rootItem = WindItem{
            QVector3D(0, -300, 0), //相对位置，y轴-300相当于放到了底部
            QVector3D(0, 0, 0), //相对方向
            root_metas,
            root_subs,
            QVector3D(0, -0.1f, 0)}; //给y加了动画因子，即柱子在xz平面转
}

void Windmill3D::drawImage(int width, int height)
{
    if (width > 10 && height > 10 && watcher.isFinished())
    {
        QQuaternion rotate = rotationQuat;
        float step = animationStep;
        float fovy = projectionFovy;

        //多线程绘制到image上，绘制完后返回image并绘制到窗口上
        QFuture<QImage> futures = QtConcurrent::run([this, width, height, rotate, step, fovy]()
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
            //抗锯齿
            painter.setRenderHint(QPainter::Antialiasing);

            //计算所有的图元顶点路径
            QList<QSharedPointer<WindMeta>> surface_metas = rootItem.calcSurfaceMetas(
                        QVector3D(0, 0, 0), rotate, step, fovy);
            //根据z轴排序
            std::sort(surface_metas.begin(), surface_metas.end(),
                      [](const QSharedPointer<WindMeta> &left, const QSharedPointer<WindMeta> &right)
            {
                return left->z < right->z;
            });
            //根据z值从远处开始绘制图元路径
            for (QSharedPointer<WindMeta> meta : qAsConst(surface_metas))
            {
                painter.fillPath(meta->path, meta->color);
            }

            //painter.restore();
            return img;
        });
        watcher.setFuture(futures);
    }
}
