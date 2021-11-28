#include "DrawImage.h"
#include <QPainter>
#include <QPainterPath>
#include <QDebug>

DrawImage::DrawImage(QWidget *parent)
    : QWidget(parent)
{
    imgCache = QImage(":/hehua.png");
    imgCache.convertTo(QImage::Format_ARGB32);
    maskCache = QImage(":/heart.png");
    maskCache.convertTo(QImage::Format_ARGB32);

    //定时器用来做动态效果
    connect(&timer, &QTimer::timeout, [this]
            {
                timeOffset++;
                update();
            });
}

void DrawImage::showEvent(QShowEvent *event)
{
    timer.start(100);
    QWidget::showEvent(event);
}

void DrawImage::hideEvent(QHideEvent *event)
{
    timer.stop();
    QWidget::hideEvent(event);
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
    //中间变量
    QImage img_a;

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
    img_a = maskCache.scaled(200, 200);
    {
        QPainter p(&img_a);
        //因为图片是放大过的，所以没法
        //p.setRenderHint(QPainter::SmoothPixmapTransform);
        //源像素混合在目标的顶部，源像素的 alpha 减去目标像素的 alpha。
        p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
        p.drawImage(0, 0, imgCache);
    }
    painter.drawImage(10 + 210 * 3, 30, img_a);
    painter.drawText(10 + 210 * 3, 20, "mask");
    //其他
    //根据给定的颜色值创建并返回此图像的蒙版
    //QImage QImage::createMaskFromColor(QRgb color, Qt::MaskMode mode = Qt::MaskInColor) const
    //函数创建并返回此图像的 1-bpp 启发式掩码
    //QImage QImage::createHeuristicMask(bool clipTight = true) const
    //从此图像中的 alpha 缓冲区构建并返回一个 1-bpp 掩码
    //QImage QImage::createMaskFromColor(QRgb color, Qt::MaskMode mode = Qt::MaskInColor) const

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
    //填充为某个颜色
    img_a = maskCache.copy();
    {
        QPainter p(&img_a);
        p.setCompositionMode(QPainter::CompositionMode_SourceIn);
        p.fillRect(img_a.rect(), Qt::green); //原谅绿
    }
    painter.drawImage(10 + 210 * 5, 30, img_a);
    img_a = maskCache.copy();
    {
        QPainter p(&img_a);
        p.setCompositionMode(QPainter::CompositionMode_SourceIn);
        p.fillRect(img_a.rect(), Qt::red); //原谅绿
    }
    painter.drawImage(10 + 210 * 5 + 100, 30 + 100, img_a);
    painter.drawText(10 + 210 * 5, 20, "colorImage");

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

    //亮度调节
    //(暖色调加减红绿，冷色调加减蓝)
    img_a = imgCache.copy();
    int brightness_offset = 100;
    int red, green, blue;
    for (int row = 0; row < img_a.height(); row++)
    {
        //整行数据,typedef unsigned int QRgb;
        QRgb *line_data = (QRgb *)img_a.scanLine(row);
        for (int col = 0; col < img_a.width(); col++)
        {
            red = qRed(line_data[col]) + brightness_offset;
            red = (red < 0x00) ? 0x00 : (red > 0xff) ? 0xff
                                                     : red;
            green = qGreen(line_data[col]) + brightness_offset;
            green = (green < 0x00) ? 0x00 : (green > 0xff) ? 0xff
                                                           : green;
            blue = qBlue(line_data[col]) + brightness_offset;
            blue = (blue < 0x00) ? 0x00 : (blue > 0xff) ? 0xff
                                                        : blue;
            line_data[col] = qRgba(red, green, blue, qAlpha(line_data[col]));
        }
    }
    painter.drawImage(10 + 210 * 2, 310, img_a);
    painter.drawText(10 + 210 * 2, 300, "brightness");

    //对比度
    img_a = imgCache.copy();
    int contrast_offset = 50;
    for (int row = 0; row < img_a.height(); row++)
    {
        //整行数据,typedef unsigned int QRgb;
        QRgb *line_data = (QRgb *)img_a.scanLine(row);
        for (int col = 0; col < img_a.width(); col++)
        {
            float param = 0;
            if (contrast_offset > 0 && contrast_offset < 100)
            {
                param = 1 / (1 - contrast_offset / 100.0) - 1;
            }
            else
            {
                param = contrast_offset / 100.0;
            }
            red = qRed(line_data[col]);
            red = red + (red - 127) * param;
            red = (red < 0x00) ? 0x00 : (red > 0xff) ? 0xff
                                                     : red;
            green = qGreen(line_data[col]);
            green = green + (green - 127) * param;
            green = (green < 0x00) ? 0x00 : (green > 0xff) ? 0xff
                                                           : green;
            blue = qBlue(line_data[col]);
            blue = blue + (blue - 127) * param;
            blue = (blue < 0x00) ? 0x00 : (blue > 0xff) ? 0xff
                                                        : blue;
            line_data[col] = qRgba(red, green, blue, qAlpha(line_data[col]));
        }
    }
    painter.drawImage(10 + 210 * 3, 310, img_a);
    painter.drawText(10 + 210 * 3, 300, "contrast");

    //饱和度
    //HSL格式表示色彩 - hue（色相）, saturation（饱和度）, lightness（明度）
    img_a = imgCache.copy();
    int hue, saturation, lightness;
    for (int row = 0; row < img_a.height(); row++)
    {
        //整行数据,typedef unsigned int QRgb;
        QRgb *line_data = (QRgb *)img_a.scanLine(row);
        for (int col = 0; col < img_a.width(); col++)
        {
            QColor color = QColor(line_data[col]).toHsl();
            hue = color.hue();
            saturation = color.saturation() + 200;
            lightness = color.lightness();
            saturation = qBound(0, saturation, 255);
            color.setHsl(hue, saturation, lightness);
            line_data[col] = color.rgba();
        }
    }
    painter.drawImage(10 + 210 * 4, 310, img_a);
    painter.drawText(10 + 210 * 4, 300, "saturation");
}
