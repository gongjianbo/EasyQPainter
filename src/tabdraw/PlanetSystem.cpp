#include "PlanetSystem.h"

#include <QPainter>
#include <QPainterPath>
#include <QTimer>

PlanetSystem::PlanetSystem(QWidget *parent)
    : QWidget(parent)
{
    initSystem();

    connect(&timer, &QTimer::timeout, this, [this]
            {
                updatePlanet(rootPlanet);
                update();
            });
}

PlanetSystem::~PlanetSystem()
{
    freeSystem();
}

void PlanetSystem::showEvent(QShowEvent *event)
{
    timer.start(50);
    QWidget::showEvent(event);
}

void PlanetSystem::hideEvent(QHideEvent *event)
{
    timer.stop();
    QWidget::hideEvent(event);
}

void PlanetSystem::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    //先画一个白底黑框
    painter.fillRect(this->rect(), Qt::white);
    QPen pen(Qt::black);
    painter.setPen(pen);
    painter.drawRect(this->rect().adjusted(0, 0, -1, -1)); //右下角会超出范围

    //移动到中心，开始绘制
    painter.translate(this->rect().center());
    drawPlanet(&painter, rootPlanet);
}

void PlanetSystem::initSystem()
{
    //PlanetNode(公转半径，公转速度，自身半径，自转速度，颜色)
    rootPlanet = new PlanetNode(0, 0.0f, 40, 1.5f, QColor(255, 0, 0));

    PlanetNode *p0 = new PlanetNode(100, 1.5f, 15, 1.5f, QColor(255, 255, 0));
    rootPlanet->subPlanet.push_back(p0);

    PlanetNode *p1 = new PlanetNode(160, 0.5f, 25, 1.5f, QColor(0, 255, 0));
    rootPlanet->subPlanet.push_back(p1);

    PlanetNode *p1_1 = new PlanetNode(50, 1.0f, 15, 1.5f, QColor(0, 0, 255));
    p1->subPlanet.push_back(p1_1);

    PlanetNode *p3 = new PlanetNode(260, 0.2f, 20, 1.5f, QColor(0, 255, 255));
    rootPlanet->subPlanet.push_back(p3);
}

void PlanetSystem::freeSystem()
{
    delete rootPlanet;
}

void PlanetSystem::drawPlanet(QPainter *painter, PlanetNode *planet)
{
    painter->rotate(planet->curSelfRotate);
    //画一个方框和一个圆，方框是为了看自转
    QRect planet_rect = QRect(QPoint(-planet->selfRadius, -planet->selfRadius),
                              QPoint(planet->selfRadius, planet->selfRadius));
    painter->setBrush(planet->color);
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(planet_rect);
    painter->setBrush(Qt::NoBrush);
    painter->setPen(Qt::black);
    painter->drawRect(planet_rect);

    //遍历子节点
    //注意：子节点的旋转和位移是相对父节点的
    for (PlanetNode *sub_planet : qAsConst(planet->subPlanet))
    {
        //公转轨迹
        painter->drawEllipse(QPoint(0, 0),
                             sub_planet->sysRadius,
                             sub_planet->sysRadius);
        painter->save();
        //位置转移到子星体处绘制
        //目前直接用的圆形轨道，可给每个星体一个轨道公式
        painter->rotate(sub_planet->curSysRotate);
        painter->translate(0, -sub_planet->sysRadius);
        drawPlanet(painter, sub_planet);
        painter->restore();
    }
}

void PlanetSystem::updatePlanet(PlanetNode *planet)
{
    //公转
    planet->curSysRotate += planet->sysSpeed;
    //planet->curSysRotate%=360;
    //自转
    planet->curSelfRotate += planet->selfSpeed;
    //planet->curSelfRotate%=360;
    //遍历子节点
    for (PlanetNode *sub_planet : qAsConst(planet->subPlanet))
    {
        updatePlanet(sub_planet);
    }
}
