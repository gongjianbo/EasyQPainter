#ifndef MYPLANETSYSTEM_H
#define MYPLANETSYSTEM_H

#include <QWidget>

//单个星体
class MyPlanet
{
public:
    //构造
    MyPlanet(int sysradius,float sysspeed,
             int selfradius,float selfspeed,
             const QColor &color)
        :sysRadius(sysradius),sysSpeed(sysspeed),
          selfRadius(selfradius),selfSpeed(selfspeed),color(color)
    {

    }
    //析构释放子节点
    ~MyPlanet()
    {
        for(MyPlanet *sub:subPlanet)
        {
            delete sub;
        }
    }
private:
    //公转半径
    const int sysRadius;
    //公转速度，为了简化，这里表示每次刷新时围绕父节点旋转的角度（不是移动位置长度）
    const float sysSpeed;
    //星体半径
    const int selfRadius;
    //自转速度，为了简化，这里表示每次刷新时旋转的角度
    const float selfSpeed;
    //当前公转的角度
    float curSysRotate=0;
    //当前自转的角度
    float curSelfRotate=0;
    //颜色
    const QColor color;
    //围绕该星体旋转的子星体
    QList<MyPlanet*> subPlanet;

    friend class MyPlanetSystem;
};

//简易的星体系统-演示rotate使用
class MyPlanetSystem : public QWidget
{
    Q_OBJECT
public:
    explicit MyPlanetSystem(QWidget *parent = nullptr);
    ~MyPlanetSystem();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void initSystem();
    void freeSystem();
    //递归绘制
    void drawPlanet(QPainter *painter,MyPlanet *planet);
    //递归更新角度
    void updatePlanet(MyPlanet *planet);

private:
    MyPlanet *rootPlanet=nullptr;
};

#endif // MYPLANETSYSTEM_H
