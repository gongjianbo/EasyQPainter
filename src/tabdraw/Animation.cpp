#include "Animation.h"
#include <cmath>
#include <QtMath>
#include <QPaintEvent>
#include <QPainter>
#include <QGradient>
#include <QDebug>

Animation::Animation(QWidget *parent)
    : QWidget(parent)
{
    // 1-1.波纹
    connect(&waveTimer, &QTimer::timeout, this, [this]
    {
        waveOffset += 4;
        waveOffset %= waveLimit;
        update();
    });
    // 2-1.色块
    blockAnimation = new QParallelAnimationGroup(this);
    QPropertyAnimation *block_color = new QPropertyAnimation(this);
    block_color->setTargetObject(this);
    block_color->setPropertyName("blockColor");
    block_color->setStartValue(QColor(255, 0, 0));
    block_color->setEndValue(QColor(255, 255, 0));
    block_color->setDuration(3000);
    blockAnimation->addAnimation(block_color);
    QPropertyAnimation *block_scare = new QPropertyAnimation(this);
    block_scare->setTargetObject(this);
    block_scare->setPropertyName("blockScare");
    block_scare->setStartValue(1.0f);
    block_scare->setEndValue(0.2f);
    block_scare->setEasingCurve(QEasingCurve::InCubic); //缓和曲线设置
    block_scare->setDuration(2000);
    blockAnimation->addAnimation(block_scare);
    //播放完了就倒着播放
    connect(blockAnimation, &QParallelAnimationGroup::finished, this, [this]
    {
        if(blockAnimation->direction() == QAbstractAnimation::Forward){
            blockAnimation->setDirection(QAbstractAnimation::Backward);
        }else{
            blockAnimation->setDirection(QAbstractAnimation::Forward);
        }
        blockAnimation->start();
    });
}

QColor Animation::getBlockColor() const
{
    return blockColor;
}

void Animation::setBlockColor(const QColor &color)
{
    blockColor = color;
    emit blockColorChanged();
    update();
}

float Animation::getBlockScare() const
{
    return blockScare;
}

void Animation::setBlockScare(float scare)
{
    blockScare = scare;
    emit blockScareChanged();
    update();
}

void Animation::showEvent(QShowEvent *event)
{
    waveTimer.start(50);
    blockAnimation->start();
    QWidget::showEvent(event);
}

void Animation::hideEvent(QHideEvent *event)
{
    waveTimer.stop();
    blockAnimation->pause();
    QWidget::hideEvent(event);
}

void Animation::paintEvent(QPaintEvent *event)
{
    event->accept();
    QPainter painter(this);
    //黑底白字
    painter.fillRect(this->rect(), Qt::black);
    painter.setPen(Qt::white);

    // 1-1.波纹
    drawWave(painter, QRect(20, 20, 200, 200));
    // 2-1.色块
    drawColorBlock(painter, QRect(240, 20, 200, 200));
}

void Animation::drawWave(QPainter &painter, const QRectF &area)
{
    painter.save();
    //波纹
    QRadialGradient gradient(area.center(), 100);
    //波纹间隔，总量为1
    const double f_space = 0.18;
    //一层波纹的内侧偏移，相对于f_offset
    const double f_inner = 0.02;
    //波峰偏移，相对于f_offset
    const double f_crest = 0.03;
    //波纹的外侧偏移，相对于f_offset
    const double f_outer = 0.06;
    //当前波纹偏移
    const double f_offset = waveOffset / double(waveLimit) * f_space;
    //波纹个数
    const int f_count = std::floor(1 / f_space);
    //波峰颜色
    QColor wave_color(255, 0, 0);
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
    painter.fillRect(area, gradient);
    //边框，画直线不用抗锯齿
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.drawRect(area);
    painter.restore();
}

void Animation::drawColorBlock(QPainter &painter, const QRectF &area)
{
    painter.save();
    QRectF block = area.adjusted(10, 10, -10, -10);
    block.setWidth(block.width() * getBlockScare());
    block.setHeight(block.height() * getBlockScare());
    block.moveCenter(area.center());
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(block, getBlockColor());
    //边框
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.drawRect(area);
    painter.restore();
}
