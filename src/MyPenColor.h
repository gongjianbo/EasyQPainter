#ifndef MYPENCOLOR_H
#define MYPENCOLOR_H

#include <QWidget>

//展示画笔/画刷颜色的使用
class MyPenColor : public QWidget
{
    Q_OBJECT
public:
    explicit MyPenColor(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    double angleOffset=0; //当前角度偏移
    double angleStep=0.01; //角度步进
};

#endif // MYPENCOLOR_H
