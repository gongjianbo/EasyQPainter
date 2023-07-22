#pragma once
#include <QWidget>
#include <QPainterPath>
#include <QTimer>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QTimeLine>

// 动画效果
class DrawAnimation : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor blockColor READ getBlockColor WRITE setBlockColor NOTIFY blockColorChanged)
    Q_PROPERTY(float blockScare READ getBlockScare WRITE setBlockScare NOTIFY blockScareChanged)
public:
    explicit DrawAnimation(QWidget *parent = nullptr);

    QColor getBlockColor() const;
    void setBlockColor(const QColor &color);

    float getBlockScare() const;
    void setBlockScare(float scare);

protected:
    // 显示时才启动定时动画
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    // 绘制
    void paintEvent(QPaintEvent *event) override;

private:
    // 定时器波纹
    void drawWave(QPainter &painter, const QRectF &area);
    // 属性动画色块
    void drawColorBlock(QPainter &painter, const QRectF &area);
    // 时间轴圆角
    void drawRadiusBlock(QPainter &painter, const QRectF &area);

signals:
    void blockColorChanged();
    void blockScareChanged();

private:
    // 1. 定时器动画
    // 1-1. 波纹
    QTimer waveTimer;
    int waveOffset{0};
    const int waveLimit{100};
    // 2. 属性动画
    // 2-1. 色块
    // 一个并行执行的动画组，同时改变颜色和大小
    QParallelAnimationGroup *blockAnimation{nullptr};
    QColor blockColor{255, 0, 0};
    float blockScare{1.0};
    // 3. 时间轴
    // 3-1. 圆角
    QTimeLine *roundTimeline{nullptr};
    int roundRaiuds{0};
};
