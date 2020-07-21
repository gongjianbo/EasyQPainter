#ifndef MYTEXTPATH_H
#define MYTEXTPATH_H

#include <QWidget>

//展示文字路径
class MyTextPath : public QWidget
{
    Q_OBJECT
public:
    explicit MyTextPath(QWidget *parent = nullptr);

protected:
    //绘图
    void paintEvent(QPaintEvent *event) override;
    //定时器
    void timerEvent(QTimerEvent * event) override;
    //窗口改变大小，可能需要重新计算
    void resizeEvent(QResizeEvent * event) override;

private:
    int textOffset_1=0;//文本移动偏移量
    int textWidth_1=1;//文本绘制宽度
    int labelWidth_1=1;//文本绘制区域宽度

    int textOffset_2=0;//文本移动偏移量
    int textWidth_2=1;//文本绘制宽度
    int labelWidth_2=1;//文本绘制区域宽度
};

#endif // MYTEXTPATH_H
