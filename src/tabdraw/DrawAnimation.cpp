#include "DrawAnimation.h"
#include <cmath>
#include <QtMath>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QGradient>
#include <QDebug>

DrawAnimation::DrawAnimation(QWidget *parent)
    : QWidget(parent)
{
    // 1-1. 波纹
    connect(&waveTimer, &QTimer::timeout, this, [this]
    {
        waveOffset += 4;
        waveOffset %= waveLimit;
        update();
    });
    // 2-1. 色块
    // 先创建一个并行属性动画组
    blockAnimation = new QParallelAnimationGroup(this);
    // 添加色块颜色动画
    QPropertyAnimation *block_color = new QPropertyAnimation(this);
    block_color->setTargetObject(this);
    block_color->setPropertyName("blockColor");
    block_color->setStartValue(blockColor);
    block_color->setEndValue(QColor(255, 255, 0));
    block_color->setDuration(3000);
    blockAnimation->addAnimation(block_color);
    // 添加色块缩放动画
    QPropertyAnimation *block_scare = new QPropertyAnimation(this);
    block_scare->setTargetObject(this);
    block_scare->setPropertyName("blockScare");
    block_scare->setStartValue(blockScare);
    block_scare->setEndValue(0.2f);
    block_scare->setEasingCurve(QEasingCurve::InCubic); // 缓和曲线设置
    block_scare->setDuration(2000);
    blockAnimation->addAnimation(block_scare);
    // 动画组播放完了就倒着继续播放
    connect(blockAnimation, &QParallelAnimationGroup::finished, this, [this]
    {
        if (blockAnimation->direction() == QAbstractAnimation::Forward)
        {
            blockAnimation->setDirection(QAbstractAnimation::Backward);
        }
        else
        {
            blockAnimation->setDirection(QAbstractAnimation::Forward);
        }
        blockAnimation->start();
    });
    // 3-1. 圆角
    roundTimeline = new QTimeLine(5000, this);
    roundTimeline->setFrameRange(0, 100);
    connect(roundTimeline, &QTimeLine::frameChanged, this, [this](int frame)
    {
        roundRaiuds = frame;
        update();
    });
    connect(roundTimeline, &QTimeLine::finished, this, [this]
    {
        if (roundTimeline->direction() == QTimeLine::Forward)
        {
            roundTimeline->setDirection(QTimeLine::Backward);
        }
        else
        {
            roundTimeline->setDirection(QTimeLine::Forward);
        }
        roundTimeline->start();
    });
}

QColor DrawAnimation::getBlockColor() const
{
    return blockColor;
}

void DrawAnimation::setBlockColor(const QColor &color)
{
    blockColor = color;
    emit blockColorChanged();
    update();
}

float DrawAnimation::getBlockScare() const
{
    return blockScare;
}

void DrawAnimation::setBlockScare(float scare)
{
    blockScare = scare;
    emit blockScareChanged();
    update();
}

void DrawAnimation::showEvent(QShowEvent *event)
{
    waveTimer.start(50);
    blockAnimation->start();
    roundTimeline->start();
    QWidget::showEvent(event);
}

void DrawAnimation::hideEvent(QHideEvent *event)
{
    waveTimer.stop();
    // Qt6 提示 QAbstractAnimation::pause: Cannot pause a stopped animation
    if (blockAnimation->state() == QAbstractAnimation::Running)
    {
        blockAnimation->pause();
    }
    // Qt6 提示 QTimeLine::setPaused: Not running
    if (roundTimeline->state() == QTimeLine::Running)
    {
        roundTimeline->setPaused(true);
    }
    QWidget::hideEvent(event);
}

void DrawAnimation::paintEvent(QPaintEvent *event)
{
    event->accept();
    QPainter painter(this);
    // 黑底白字
    painter.fillRect(this->rect(), Qt::black);
    // 1-1. 波纹
    drawWave(painter, QRect(20, 20, 200, 200));
    // 2-1. 色块
    drawColorBlock(painter, QRect(240, 20, 200, 200));
    // 3-1. 圆角
    drawRadiusBlock(painter, QRect(460, 20, 200, 200));
}

void DrawAnimation::drawWave(QPainter &painter, const QRectF &area)
{
    painter.save();
    // 波纹
    QRadialGradient gradient(area.center(), 100);
    // 波纹间隔，总量为 1
    const double f_space = 0.18;
    // 一层波纹的内侧偏移，相对于 f_offset
    const double f_inner = 0.02;
    // 波峰偏移，相对于 f_offset
    const double f_crest = 0.03;
    // 波纹的外侧偏移，相对于 f_offset
    const double f_outer = 0.06;
    // 当前波纹偏移
    const double f_offset = waveOffset / double(waveLimit) * f_space;
    // 波纹个数
    const int f_count = std::floor(1 / f_space);
    // 波峰颜色
    QColor wave_color(255, 0, 0);
    gradient.setColorAt(0, Qt::transparent);
    for (int i = 0; i < f_count; i++)
    {
        if (f_offset + i * f_space > f_inner)
        {
            gradient.setColorAt(f_offset + i * f_space - f_inner, Qt::transparent);
        }
        // 最后一个波纹透明度逐渐为 0
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
    // 波纹抗锯齿
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(area, gradient);
    // 边框，画直线不用抗锯齿
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(Qt::white);
    painter.drawRect(area);
    painter.restore();
}

void DrawAnimation::drawColorBlock(QPainter &painter, const QRectF &area)
{
    painter.save();
    // 根据当前属性颜色和缩放比例绘制色块
    QRectF block = area.adjusted(10, 10, -10, -10);
    block.setWidth(block.width() * getBlockScare());
    block.setHeight(block.height() * getBlockScare());
    block.moveCenter(area.center());
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(block, getBlockColor());
    // 边框
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(Qt::white);
    painter.drawRect(area);
    painter.restore();
}

void DrawAnimation::drawRadiusBlock(QPainter &painter, const QRectF &area)
{
    painter.save();
    // 根据当前属性颜色和缩放比例绘制色块
    QRectF block = area.adjusted(10, 10, -10, -10);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::cyan);
    QPainterPath round_path;
    round_path.addRoundedRect(block, roundRaiuds, roundRaiuds);
    painter.drawPath(round_path);
    // 边框
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(Qt::white);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(area);
    painter.restore();
}
