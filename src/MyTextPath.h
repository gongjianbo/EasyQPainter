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
    //窗口改变大小，可能需要重新计算
    void resizeEvent(QResizeEvent * event) override;

private:
    int areaWidth=350;//左侧文字区域宽度
    bool areaAdd=false;//控制宽度变大还是变小
    //拉伸之后Qt计算可能有点问题
    //有些字号更大了，但是拉伸后反而变窄了
    //所以保持上次的字体大小和宽度，异常则使用上次的字体
    //使之增加时不能小于上次，减小时不能大于上次
    int prevWidth=0;//上次文本宽
    int prevSize=0;//上次字号
    int prevStretch=0;//上次拉伸因子

    int textOffset_1=0;//文本移动偏移量
    int textWidth_1=1;//文本绘制宽度
    int labelWidth_1=1;//文本绘制区域宽度

    int textOffset_2=0;//文本移动偏移量
    int textWidth_2=1;//文本绘制宽度
    int labelWidth_2=1;//文本绘制区域宽度
};

#endif // MYTEXTPATH_H
