#ifndef MYDRAWIMAGE_H
#define MYDRAWIMAGE_H

#include <QWidget>
#include <QImage>
#include <QComboBox>

//展示图片的绘制
class MyDrawImage : public QWidget
{
    Q_OBJECT
public:
    explicit MyDrawImage(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    //普通绘制image
    void drawNormal(QPainter *painter);
    //灰度图
    void drawGray(QPainter *painter);

private:
    //有两张图
    QImage imgA;
    QImage imgB;
    //切换绘制效果
    QComboBox *comboBox;
    //定时器动画
    int timeOffset=0;
};

#endif // MYDRAWIMAGE_H
