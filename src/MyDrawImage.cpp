#include "MyDrawImage.h"

#include <QPainter>
#include <QPainterPath>
#include <QTimer>
#include <QListView>
#include <QDebug>

MyDrawImage::MyDrawImage(QWidget *parent)
    : QWidget(parent),
      comboBox(new QComboBox(this))
{
    //定时器用来做动态效果
    QTimer *timer=new QTimer(this);
    connect(timer,&QTimer::timeout,[=]{
        timeOffset++;

        if(isHidden())
            return;
        update();
    });
    timer->start(100);

    imgA=QImage(":/img/xingkong.jpg");
    imgB=QImage(":/img/xiangrikui.jpg");

    //下拉框
    comboBox->setView(new QListView(this));
    comboBox->addItems({
                           "Normal", //一些常用方法
                           "Gray" //灰度图
                       });
    //切换combobox时刷新显示
    connect(comboBox,QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this]{
        this->update();
    });
}

void MyDrawImage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    //先画一个白底黑框
    painter.fillRect(this->rect(),Qt::white);
    painter.setPen(QPen(Qt::black,1,Qt::DashLine));
    painter.drawRect(this->rect().adjusted(0,0,-1,-1)); //右下角会超出范围

    //根据选项绘制
    switch(comboBox->currentIndex()){
    case 0:
        drawNormal(&painter);
        break;
    case 1:
        drawGray(&painter);
        break;
    default: break;
    }
}

void MyDrawImage::drawNormal(QPainter *painter)
{
    //painter移动到中心
    painter->translate(this->rect().center());
    //红色笔
    painter->setPen(Qt::red);

    //drawImage是以左上角为起点，drawText是以左下角为起点
    //缩放
    painter->drawImage(-300,-180,imgA.scaled(100,100));
    painter->drawText(-300,-180-10,"scaled");
    //镜像，可以横项和纵向反转
    painter->drawImage(-150,-180,imgA.scaled(100,100).mirrored(true,true));
    painter->drawText(-150,-180-10,"mirrored");
    //红蓝分量交换
    painter->drawImage(0,-180,imgA.scaled(200,150).rgbSwapped());
    painter->drawText(0,-180-10,"rgbSwapped");
    //mask
    //createMaskFromColor，需要执行的颜色来制作蒙版，懒得找图
    //createAlphaMask需要透明度，也懒得找图

    QImage img_a;
    //转换矩阵
    QTransform mat_a; //3*3矩阵，貌似用QMatrix也可以
    mat_a.scale(0.2,0.2); //缩放
    img_a=imgA.transformed(mat_a,Qt::SmoothTransformation);
    painter->drawImage(-300,0,img_a);
    painter->drawRect(QRect(QPoint(-300,0),img_a.rect().size()));
    mat_a.rotate(timeOffset*5%360); //旋转
    img_a=imgA.transformed(mat_a,Qt::SmoothTransformation);
    painter->drawImage(-300,0,img_a);
    painter->drawRect(QRect(QPoint(-300,0),img_a.rect().size()));
    painter->drawText(-300,0-10,"transformed");
    //可以发现，用矩阵转换的话就没法像转换painter坐标那样获取到矩形的实际边了
    //一般我用painter的rotate

    //转换矩阵
    QTransform mat_c; //矩阵
    mat_c.scale(0.2,0.2); //缩放
    img_a=imgA.transformed(mat_c,Qt::SmoothTransformation);
    painter->drawImage(0,0,img_a);
    painter->drawRect(QRect(QPoint(0,0),img_a.rect().size()));
    //mat_c.translate(0.5,0.5); //me没效果
    mat_c.shear(0.2,0.5); //剪切变形，菱形的效果
    img_a=imgA.transformed(mat_c,Qt::SmoothTransformation);
    painter->drawImage(0,0,img_a);
    painter->drawRect(QRect(QPoint(0,0),img_a.rect().size()));
    painter->drawText(0,0-10,"transformed");
}

void MyDrawImage::drawGray(QPainter *painter)
{
    //转换为argb32格式：(0xAARRGGBB).
    QImage img_a=imgA.convertToFormat(QImage::Format_ARGB32);
    for(int row=0; row<img_a.height(); row++)
    {
        //整行数据,typedef unsigned int QRgb;
        QRgb *line_data=(QRgb *)img_a.scanLine(row);
        int calc_gray;
        for(int col=0; col<img_a.width(); col++)
        {
            //多种计算方式，效果略有差别
            // Gray = R*0.299 + G*0.587 + B*0.114
            // Gray = (R*19595 + G*38469 + B*7472) >> 16
            // Gray = (R * 11 + G * 16 + B * 5)/32;（qGray）
            //calc_gray=qGray(line_data[col]);
            calc_gray=qGray(line_data[col]);
            //渐变效果，0是黑色，255白色，可以改变大于小于
            if((timeOffset*5%255)>calc_gray){
                line_data[col]=qRgb(calc_gray,calc_gray,calc_gray);
            }
        }
    }
    painter->drawImage(this->rect(),img_a,img_a.rect());
}
