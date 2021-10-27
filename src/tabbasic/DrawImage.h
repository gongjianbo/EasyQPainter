#pragma once
#include <QWidget>
#include <QImage>
#include <QComboBox>
#include <QTimer>

//图片绘制相关
//有趣的链接：
//https://www.cnblogs.com/aslistener/articles/4478303.html
//https://www.cnblogs.com/swarmbees/p/5722882.html
class DrawImage : public QWidget
{
    Q_OBJECT
public:
    explicit DrawImage(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    //定时器动画
    QTimer timer;
    int timeOffset{0};
    //待绘制的图
    QImage imgCache;
};
