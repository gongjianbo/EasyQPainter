#include "MyPlanetSystem.h"

#include <QPainter>
#include <QPainterPath>
#include <QTimer>

MyPlanetSystem::MyPlanetSystem(QWidget *parent)
    : QWidget(parent)
{
    initSystem();

    QTimer *timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,[=]{
        updatePlanet(rootPlanet);

        if(isHidden())
            return;
        update();
    });
    timer->start(100);
}

MyPlanetSystem::~MyPlanetSystem()
{
    freeSystem();
}

void MyPlanetSystem::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    //先画一个白底黑框
    painter.fillRect(this->rect(),Qt::white);
    QPen pen(Qt::black);
    painter.setPen(pen);
    painter.drawRect(this->rect().adjusted(0,0,-1,-1)); //右下角会超出范围

    //移动到中心，开始绘制
    painter.translate(this->rect().center());
    drawPlanet(&painter,rootPlanet);
}

void MyPlanetSystem::initSystem()
{
    //MyPlanet(公转半径，公转速度，自身半径，自转速度，颜色)
    rootPlanet=new MyPlanet(0,0,40,3,QColor(255,0,0));

    MyPlanet *p0=new MyPlanet(100,3,15,3,QColor(255,255,0));
    rootPlanet->subPlanet.push_back(p0);

    MyPlanet *p1=new MyPlanet(160,1,25,3,QColor(0,255,0));
    rootPlanet->subPlanet.push_back(p1);

    MyPlanet *p1_1=new MyPlanet(50,2,15,3,QColor(0,0,255));
    p1->subPlanet.push_back(p1_1);

    MyPlanet *p3=new MyPlanet(260,0.3f,20,3,QColor(0,255,255));
    rootPlanet->subPlanet.push_back(p3);
}

void MyPlanetSystem::freeSystem()
{
    delete rootPlanet;
}

void MyPlanetSystem::drawPlanet(QPainter *painter, MyPlanet *planet)
{
    painter->rotate(planet->curSelfRotate);
    //画一个方框和一个圆，方框是为了看自转
    QRect planet_rect=QRect(QPoint(-planet->selfRadius,-planet->selfRadius),
                            QPoint(planet->selfRadius,planet->selfRadius));
    painter->setBrush(planet->color);
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(planet_rect);
    painter->setBrush(Qt::NoBrush);
    painter->setPen(Qt::black);
    painter->drawRect(planet_rect);

    //遍历子节点
    //注意：子节点的旋转和位移是相对父节点的
    for(MyPlanet *sub_planet:planet->subPlanet)
    {
        //公转轨迹
        painter->drawEllipse(QPoint(0,0),
                             sub_planet->sysRadius,
                             sub_planet->sysRadius);
        painter->save();
        //位置转移到子星体处绘制
        //目前直接用的圆形轨道，可给每个星体一个轨道公式
        painter->rotate(sub_planet->curSysRotate);
        painter->translate(0,-sub_planet->sysRadius);
        drawPlanet(painter,sub_planet);
        painter->restore();
    }
}

void MyPlanetSystem::updatePlanet(MyPlanet *planet)
{
    //公转
    planet->curSysRotate+=planet->sysSpeed;
    //planet->curSysRotate%=360;
    //自转
    planet->curSelfRotate+=planet->selfSpeed;
    //planet->curSelfRotate%=360;
    //遍历子节点
    for(MyPlanet *sub_planet:planet->subPlanet)
    {
        updatePlanet(sub_planet);
    }
}
