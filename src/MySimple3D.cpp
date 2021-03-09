#include "MySimple3D.h"

#include <QPaintEvent>
#include <QResizeEvent>
#include <QPainter>
#include <QPainterPath>
#include <QtMath>
#include <algorithm>

#include <QTimer>
#include <QtConcurrent>
#include <QDebug>

MySimple3D::MySimple3D(QWidget *parent)
    : QWidget(parent)
{
    initItems();

    //异步处理结束，获取结果并刷新窗口
    connect(&watcher,&QFutureWatcher<QImage>::finished,[this](){
        image=watcher.result();
        update();
    });

    fpsTime=QTime::currentTime();
    fpsTime.start();
    //定时旋转风车
    QTimer *timer=new QTimer(this);
    connect(timer,&QTimer::timeout,[=]{
        animationStep+=2.0;

        if(isHidden())
            return;
        drawImage(width(),height());
    });
    timer->start(50);
}

MySimple3D::~MySimple3D()
{
    if(!watcher.isFinished())
        watcher.waitForFinished();
}

void MySimple3D::paintEvent(QPaintEvent *event)
{
    event->accept();
    QPainter painter(this);
    painter.fillRect(this->rect(),Qt::black);

    if(image.size().isValid())
        painter.drawImage(0,0,image);

    //fps统计
    if(fpsTime.elapsed()>1000){
        fpsTime.restart();
        fpsCounter=fpsTemp;
        fpsTemp=0;
    }else{
        fpsTemp++;
    }
    painter.setPen(QPen(Qt::white));
    painter.drawText(10,30,"FPS:"+QString::number(fpsCounter));
    painter.drawText(10,50,"Drag Moving ... ...");
}

void MySimple3D::mousePressEvent(QMouseEvent *event)
{
    mousePressed=true;
    mousePos=event->pos();
    QWidget::mousePressEvent(event);
}

void MySimple3D::mouseMoveEvent(QMouseEvent *event)
{
    if(mousePressed){
        const QPoint posOffset=event->pos()-mousePos;
        mousePos=event->pos();
        //旋转矩阵  x和y分量
        xRotate+=-posOffset.y();
        yRotate+=-posOffset.x();
        //update();
        drawImage(width(),height());
    }
    QWidget::mouseMoveEvent(event);
}

void MySimple3D::mouseReleaseEvent(QMouseEvent *event)
{
    mousePressed=false;
    QWidget::mouseReleaseEvent(event);
}

void MySimple3D::resizeEvent(QResizeEvent *event)
{
    if(event->size().isValid()){
        const int width=event->size().width();
        const int height=event->size().height();
        drawImage(width,height);
    }
    QWidget::resizeEvent(event);
}

void MySimple3D::initItems()
{
    //模板的嵌套时自动格式化太难看了
    //四个扇叶
    My3DMeta* sub_fan1=new My3DMeta{{
            QVector3D(0,0,0),QVector3D(-250,250,0),QVector3D(-300,200,10),QVector3D(-100,0,10)},
            QColor(110,250,250,200)};
    My3DMeta* sub_fan2=new My3DMeta{{
            QVector3D(0,0,0),QVector3D(-250,-250,0),QVector3D(-200,-300,10),QVector3D(0,-100,10)},
            QColor(130,250,250,200)};
    My3DMeta* sub_fan3=new My3DMeta{{
            QVector3D(0,0,0),QVector3D(250,-250,0),QVector3D(300,-200,10),QVector3D(100,0,10)},
            QColor(110,250,250,200)};
    My3DMeta* sub_fan4=new My3DMeta{{
            QVector3D(0,0,0),QVector3D(250,250,0),QVector3D(200,300,10),QVector3D(0,100,10)},
            QColor(130,250,250,200)};
    auto sub_fanmetas=QList<QSharedPointer<My3DMeta>>{QSharedPointer<My3DMeta>(sub_fan1),
                                                     QSharedPointer<My3DMeta>(sub_fan2),
                                                     QSharedPointer<My3DMeta>(sub_fan3),
                                                     QSharedPointer<My3DMeta>(sub_fan4)};
    auto sub_fansubs=QList<QSharedPointer<My3DItem>>{};
    My3DItem *sub_fanitem=new My3DItem{
            QVector3D(0,400,-150),
            QVector3D(0,0,0),
            sub_fanmetas,
            sub_fansubs,
            QVector3D(0,0,-1)}; //给z加了动画因子

    //风车主干，共9个面，顶部尖塔4+主干4+底面
    My3DMeta* sub_main1=new My3DMeta{{
            QVector3D(100,400,100),QVector3D(-100,400,100),QVector3D(0,500,0)},
            QColor(250,0,0)};
    My3DMeta* sub_main2=new My3DMeta{{
            QVector3D(-100,400,100),QVector3D(-100,400,-100),QVector3D(0,500,0)},
            QColor(0,250,0)};
    My3DMeta* sub_main3=new My3DMeta{{
            QVector3D(-100,400,-100),QVector3D(100,400,-100),QVector3D(0,500,0)},
            QColor(0,0,250)};
    My3DMeta* sub_main4=new My3DMeta{{
            QVector3D(100,400,-100),QVector3D(100,400,100),QVector3D(0,500,0)},
            QColor(250,250,0)};
    My3DMeta* sub_main5=new My3DMeta{{
            QVector3D(100,400,100),QVector3D(-100,400,100),QVector3D(-120,0,120),QVector3D(120,0,120)},
            QColor(205,150,100)};
    My3DMeta* sub_main6=new My3DMeta{{
            QVector3D(-100,400,100),QVector3D(-100,400,-100),QVector3D(-120,0,-120),QVector3D(-120,0,120)},
            QColor(220,150,100)};
    My3DMeta* sub_main7=new My3DMeta{{
            QVector3D(-100,400,-100),QVector3D(100,400,-100),QVector3D(120,0,-120),QVector3D(-120,0,-120)},
            QColor(235,150,100)};
    My3DMeta* sub_main8=new My3DMeta{{
            QVector3D(100,400,-100),QVector3D(100,400,100),QVector3D(120,0,120),QVector3D(120,0,-120)},
            QColor(250,150,100)};
    My3DMeta* sub_main9=new My3DMeta{{
            QVector3D(-120,0,120),QVector3D(-120,0,-120),QVector3D(120,0,-120),QVector3D(120,0,120)},
            QColor(200,150,0)};


    auto sub_mainmetas=QList<QSharedPointer<My3DMeta>>{QSharedPointer<My3DMeta>(sub_main1),
                                                      QSharedPointer<My3DMeta>(sub_main2),
                                                      QSharedPointer<My3DMeta>(sub_main3),
                                                      QSharedPointer<My3DMeta>(sub_main4),
                                                      QSharedPointer<My3DMeta>(sub_main5),
                                                      QSharedPointer<My3DMeta>(sub_main6),
                                                      QSharedPointer<My3DMeta>(sub_main7),
                                                      QSharedPointer<My3DMeta>(sub_main8),
                                                      QSharedPointer<My3DMeta>(sub_main9)};
    auto sub_mainsubs=QList<QSharedPointer<My3DItem>>{QSharedPointer<My3DItem>(sub_fanitem)};
    My3DItem *sub_mainitem=new My3DItem{
            QVector3D(0,0,0),
            QVector3D(0,0,0),
            sub_mainmetas,
            sub_mainsubs};

    //根节点，一个平面，（平面用半透明是为了穿模时看起来没那么别扭）
    My3DMeta* root_meta=new My3DMeta{{
            QVector3D(-200,0,200),QVector3D(200,0,200),
            QVector3D(200,0,-200),QVector3D(-200,0,-200)},
            QColor(255,255,255,100)};
    auto root_metas=QList<QSharedPointer<My3DMeta>>{QSharedPointer<My3DMeta>(root_meta)};
    auto root_subs=QList<QSharedPointer<My3DItem>>{QSharedPointer<My3DItem>(sub_mainitem)};
    rootItem=My3DItem{
            QVector3D(0,-300,0),
            QVector3D(0,0,0),
            root_metas,
            root_subs,
            QVector3D(0,0.1f,0)}; //给y加了动画因子
}

void MySimple3D::drawImage(int width, int height)
{
    if(width>10&&height>10&&watcher.isFinished()){
        QVector3D rotate=QVector3D(xRotate,yRotate,0);
        int step=animationStep;

        //多线程绘制到image上，绘制完后返回image并绘制到窗口上
        QFuture<QImage> futures=QtConcurrent::run([this,width,height,rotate,step](){
            QImage img(width,height,QImage::Format_ARGB32);
            img.fill(Qt::transparent);
            QPainter painter(&img);
            if(!painter.isActive())
                return img;
            painter.fillRect(img.rect(),Qt::black);

            //painter.save();
            //坐标原点移动到中心
            painter.translate(width/2,height/2);

            //计算所有的图元顶点路径
            QList<QSharedPointer<My3DMeta>> surface_metas=rootItem.calculateSurfaceMetas(QVector3D(0,0,0),rotate,step);
            //根据z轴排序
            std::sort(surface_metas.begin(),surface_metas.end(),
                      [](const QSharedPointer<My3DMeta> &left,const QSharedPointer<My3DMeta> &right){
                return left->z<right->z;
            });
            //根据z值从远处开始绘制图元路径
            for(QSharedPointer<My3DMeta> meta:surface_metas)
            {
                painter.fillPath(meta->path,meta->color);
            }

            //painter.restore();
            return img;
        });
        watcher.setFuture(futures);
    }
}
