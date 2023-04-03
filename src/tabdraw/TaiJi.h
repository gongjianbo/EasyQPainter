#pragma once
#include <QWidget>
#include <QTimer>

// 太极
class TaiJi : public QWidget
{
    Q_OBJECT
public:
    explicit TaiJi(QWidget *parent = nullptr);

protected:
    // 显示时才启动定时动画
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    // 绘制
    void paintEvent(QPaintEvent *event) override;
    // 逻辑验证
    void taijiTest();
    // 太极绘制
    void taijiPaint();

private:
    // 定时动画
    QTimer timer;
    // 旋转角度 [0-360]
    int offset{0};
};
