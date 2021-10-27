#include "DrawImage.h"
#include <QPainter>
#include <QPainterPath>
#include <QDebug>

DrawImage::DrawImage(QWidget *parent)
    : QWidget(parent)
{
    imgCache = QImage(":/hehua.png");
    imgCache.convertTo(QImage::Format_ARGB32);

    //定时器用来做动态效果
    connect(&timer, &QTimer::timeout, [this]
            {
                timeOffset++;

                if (isHidden())
                    return;
                update();
            });
    timer.start(100);
}

void DrawImage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    //先画一个白底黑框
    painter.fillRect(this->rect(), Qt::white);
    painter.setPen(QPen(Qt::black, 1, Qt::DashLine));
    painter.drawRect(this->rect().adjusted(0, 0, -1, -1)); //右下角会超出范围

    //红色笔
    painter.setPen(Qt::red);

    //drawImage是以左上角为起点，drawText是以左下角为起点
    //缩放
    painter.drawImage(10 + 210 * 0, 30, imgCache.scaled(100, 100));
    painter.drawImage(QRectF(10 + 210 * 0 + 100, 30 + 100, 100, 100),
                      imgCache.scaled(100, 100));
    painter.drawText(10, 20, "scaled");
    //镜像，可以横项和纵向反转
    painter.drawImage(10 + 210 * 1, 30, imgCache.mirrored(true, true));
    painter.drawText(10 + 210 * 1, 20, "mirrored");
    //红蓝分量交换
    painter.drawImage(10 + 210 * 2, 30, imgCache.rgbSwapped());
    painter.drawText(10 + 210 * 2, 20, "rgbSwapped");
    //mask
    //createMaskFromColor，需要执行的颜色来制作蒙版，懒得找图
    //createAlphaMask需要透明度，也懒得找图

    QImage img_a;
    //灰度图
    //灰度值,argb32格式：(0xAARRGGBB).
    img_a = imgCache.scaled(100, 100);
    for (int row = 0; row < img_a.height(); row++)
    {
        //整行数据,typedef unsigned int QRgb;
        QRgb *line_data = (QRgb *)img_a.scanLine(row);
        int calc_gray;
        for (int col = 0; col < img_a.width(); col++)
        {
            //多种计算方式，效果略有差别
            // Gray = R*0.299 + G*0.587 + B*0.114
            // Gray = (R*19595 + G*38469 + B*7472) >> 16
            // Gray = (R * 11 + G * 16 + B * 5)/32;（qGray）
            //calc_gray=qGray(line_data[col]);
            calc_gray = qGray(line_data[col]);
            //渐变效果，0是黑色，255白色，可以改变大于小于
            if ((timeOffset * 5 % 255) > calc_gray)
            {
                line_data[col] = qRgb(calc_gray, calc_gray, calc_gray);
            }
        }
    }
    painter.drawImage(10 + 210 * 4, 30, img_a);
    //
    img_a = imgCache.scaled(100, 100).convertToFormat(QImage::Format_Grayscale8);
    painter.drawImage(10 + 210 * 4 + 100, 30 + 100, img_a);
    painter.drawText(10 + 210 * 4, 20, "gray");

    //转换矩阵
    QTransform mat_a;      //3*3矩阵，貌似用QMatrix也可以
    mat_a.scale(0.5, 0.5); //缩放
    img_a = imgCache.transformed(mat_a, Qt::SmoothTransformation);
    painter.drawImage(10, 310, img_a);
    painter.drawRect(QRect(QPoint(10, 310), img_a.rect().size()));
    mat_a.rotate(timeOffset * 5 % 360); //旋转
    img_a = imgCache.transformed(mat_a, Qt::SmoothTransformation);
    painter.drawImage(10, 310, img_a);
    painter.drawRect(QRect(QPoint(10, 310), img_a.rect().size()));
    painter.drawText(10, 300, "transform rotate");
    //可以发现，用矩阵转换的话就没法像转换painter坐标那样获取到矩形的实际边了
    //一般我用painter的rotate

    //转换矩阵
    QTransform mat_c;      //矩阵
    mat_c.scale(0.5, 0.5); //缩放
    img_a = imgCache.transformed(mat_c, Qt::SmoothTransformation);
    painter.drawImage(200, 310, img_a);
    painter.drawRect(QRect(QPoint(200, 310), img_a.rect().size()));
    //mat_c.translate(0.5,0.5); //没效果
    //剪切变形，菱形的效果，为0时在原地，为1就拉伸了一倍的长/宽
    mat_c.shear((timeOffset % 100) / 100.0, 0.1);
    img_a = imgCache.transformed(mat_c, Qt::SmoothTransformation);
    painter.drawImage(200, 310, img_a);
    painter.drawRect(QRect(QPoint(200, 310), img_a.rect().size()));
    painter.drawText(200, 300, "transform shear");
}
