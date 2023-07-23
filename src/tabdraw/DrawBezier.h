#pragma once
#include <QWidget>
#include <QTimer>

// 画贝塞尔曲线
class DrawBezier : public QWidget
{
    Q_OBJECT
public:
    explicit DrawBezier(QWidget *parent = nullptr);

protected:
    // 显示时才启动定时动画
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    // 绘制
    void paintEvent(QPaintEvent *event) override;

private:
    // 定时动画
    QTimer timer;
    // 进度 [0-100]
    int theProgress{0};
};
