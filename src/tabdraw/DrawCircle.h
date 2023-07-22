#pragma once
#include <QWidget>
#include <QTimer>

// 画圆
class DrawCircle : public QWidget
{
    Q_OBJECT
public:
    explicit DrawCircle(QWidget *parent = nullptr);

protected:
    // 显示时才启动定时动画
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    // 绘制
    void paintEvent(QPaintEvent *event) override;

private:
    // 定时动画
    QTimer timer;
    // 旋转的角度 [0-360]
    int theRotate{0};
};
