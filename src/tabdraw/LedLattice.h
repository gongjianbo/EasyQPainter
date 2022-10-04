#pragma once
#include <QWidget>
#include <QTimer>

// LED点阵
class LedLattice : public QWidget
{
    Q_OBJECT
public:
    explicit LedLattice(QWidget *parent = nullptr);

protected:
    //显示时才启动定时动画
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    //绘制
    void paintEvent(QPaintEvent *event) override;

private:
    //定时动画
    QTimer timer;
    //总的列数
    static const int colCount{16};
    //当前扫描的列
    int currentCol{0};
    //横向移动偏移
    int colOffset{0};
};
