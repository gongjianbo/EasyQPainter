#include "MyTextPath.h"

#include <QPainter>
#include <QLinearGradient>
#include <QDebug>

MyTextPath::MyTextPath(QWidget *parent)
    : QWidget(parent)
{

}

void MyTextPath::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    //先画一个白底黑框
    painter.fillRect(this->rect(),Qt::white);
    QPen pen(Qt::black);
    painter.setPen(pen);
    painter.drawRect(this->rect().adjusted(0,0,-1,-1)); //右下角会超出范围

    //最短边
    const int border_width=width()>height()?height():width();
    //painter.save();

    //中心绘制一个Qt文字
    QColor qt_color(200,250,200); //淡淡的原谅色
    QFont qt_font("Microsoft YaHei",border_width/2);
    QPen qt_pen(qt_color,border_width/20+1);
    painter.setFont(qt_font);
    painter.setPen(qt_pen);
    const QString qt_str="Qt";
    //文本的pos是在左下角
    const int qt_str_width=painter.fontMetrics().width(qt_str);
    const int qt_str_height=painter.fontMetrics().height();
    //ascent从基线到字符延伸到的最高位置的距离
    //descent从基线到最低点字符延伸到的距离
    const int qt_str_descent=painter.fontMetrics().descent();
    const int qt_left=(width()-qt_str_width)/2;
    const int qt_bottom=(height()+qt_str_height)/2;
    const int qt_bottom_real=(height()+qt_str_height)/2-qt_str_descent;
    const QRect qt_rect=QRect(QPoint(qt_left,qt_bottom-qt_str_height),
                              QPoint(qt_left+qt_str_width,qt_bottom));
    //绘制文字的时候，如果使用rect指定位置，那么就在矩形中间绘制，可以指定位置flags
    //如果使用xy指定位置，会受到基线等影响，上下没对齐，要计算基线的偏移
    //painter.drawText(qt_rect,qt_str);
    //painter.drawText(qt_left,qt_bottom_real,qt_str);
    QPainterPath qt_str_path;
    qt_str_path.addText(qt_left,qt_bottom_real,qt_font,qt_str);
    //画路径
    painter.setPen(QPen(qt_color,2,Qt::DashLine));
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawPath(qt_str_path);
    painter.drawRect(qt_rect);
    //painter.restore();

    //左上角绘制文字
    const QString str_1="一切都是没有结局的开始";
    const QString str_2="一切都是稍纵即逝的追寻";
    const QFont str_font("Microsoft YaHei",28);
    const int str_height=QFontMetrics(str_font).height();
    QPainterPath str_path;
    //暂时用的固定值坐标
    str_path.addText(20,str_height,str_font,str_1);
    str_path.addText(20,str_height*2,str_font,str_2);

    painter.setPen(QPen(Qt::green,0.5));

    QLinearGradient str_gradient(0,0,
                                 0,str_height*2);
    //百分比用值计算好点，我这里手动调的固定值
    str_gradient.setColorAt(0.0,Qt::green);
    str_gradient.setColorAt(0.35,Qt::green);
    str_gradient.setColorAt(0.351,Qt::transparent);
    str_gradient.setColorAt(0.85,Qt::transparent);
    str_gradient.setColorAt(0.851,Qt::green);
    str_gradient.setColorAt(1.0,Qt::green);
    painter.drawPath(str_path);
    painter.fillPath(str_path,str_gradient);
}
