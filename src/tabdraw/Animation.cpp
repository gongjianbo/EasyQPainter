#include "Animation.h"
#include <cmath>
#include <QtMath>
#include <QPaintEvent>
#include <QPainter>
#include <QGradient>

Animation::Animation(QWidget *parent)
    : QWidget(parent)
{
    //1-1.波纹
    connect(&waveTimer, &QTimer::timeout, this, [this]
    {
        waveOffset += 4;
        waveOffset %= waveLimit;
        update();
    });
}

void Animation::showEvent(QShowEvent *event)
{
    waveTimer.start(50);
    QWidget::showEvent(event);
}

void Animation::hideEvent(QHideEvent *event)
{
    waveTimer.stop();
    QWidget::hideEvent(event);
}

void Animation::paintEvent(QPaintEvent *event)
{
    event->accept();
    QPainter painter(this);
    //黑底白字
    painter.fillRect(this->rect(), Qt::black);
    painter.setPen(Qt::white);

    //1-1.波纹-圆心(200,200)，半径(100)
    QRect wave_rect(100, 100, 200, 200);
    QRadialGradient gradient(wave_rect.center(), 100);
    const double f_space = 0.18;                                      //波纹间隔，总量为1
    const double f_inner = 0.05;                                      //一层波纹的内侧偏移，相对于f_offset
    const double f_crest = 0.03;                                      //波峰偏移，相对于f_offset
    const double f_outer = 0.06;                                      //波纹的外侧偏移，相对于f_offset
    const double f_offset = waveOffset / double(waveLimit) * f_space; //当前波纹偏移
    const int f_count = std::floor(1 / f_space);                      //波纹个数
    QColor wave_color(255, 0, 0);                                     //波峰颜色
    gradient.setColorAt(0, Qt::transparent);
    for (int i = 0; i < f_count; i++)
    {
        if (f_offset + i * f_space > f_inner)
        {
            gradient.setColorAt(f_offset + i * f_space - f_inner, Qt::transparent);
        }
        //最后一个波纹透明度逐渐为0
        if (i == f_count - 1)
        {
            QColor temp_color = wave_color;
            temp_color.setAlpha((1.0 - f_offset / f_space) * 250);
            gradient.setColorAt(f_offset + i * f_space + f_crest, temp_color);
        }
        else
        {
            gradient.setColorAt(f_offset + i * f_space + f_crest, wave_color);
        }
        gradient.setColorAt(f_offset + i * f_space + f_outer, Qt::transparent);
    }
    //波纹抗锯齿
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(wave_rect, gradient);
    //画直线不用抗锯齿
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.drawRect(wave_rect);
}
