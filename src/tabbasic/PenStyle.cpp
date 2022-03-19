#include "PenStyle.h"

#include <QPainter>
#include <QPainterPath>

PenStyle::PenStyle(QWidget *parent)
    : QWidget(parent)
{
    //累计dash总长
    for (int item : customDash)
    {
        dashCount += item;
    }

    //定时移动虚线偏移，制作蚂蚁线效果
    connect(&timer, &QTimer::timeout, this, [this]()
    {
        //虚线移动
        ++dashOffset;
        //模以dash总长度，防止越界等
        dashOffset %= dashCount;
        update();
    });
}

void PenStyle::showEvent(QShowEvent *event)
{
    timer.start(150);
    QWidget::showEvent(event);
}

void PenStyle::hideEvent(QHideEvent *event)
{
    timer.stop();
    QWidget::hideEvent(event);
}

void PenStyle::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    //先画一个白底黑框
    painter.fillRect(this->rect(), Qt::white);
    QPen pen(Qt::black);
    //SquareCap（默认）方形线端，覆盖端点（在端点延伸了半个线宽）
    //FlatCap方形线端，不覆盖端点
    //RoundCap圆角线端，覆盖端点
    pen.setCapStyle(Qt::SquareCap);
    //BevelJoin（默认）两线相交处截平
    //MiterJoin两线相交处填充三角延申
    //RoundJoin相交处圆角
    pen.setJoinStyle(Qt::MiterJoin);
    painter.setPen(pen);
    //右下角会超出范围
    painter.drawRect(this->rect().adjusted(0, 0, -1, -1));

    //准备路径
    //item是元素可视大小,rect是元素整体整体占位大小
    const int rect_width = this->width() / 3; //（3*2个圆）
    const int rect_height = this->height() / 2;
    const int center_x = rect_width / 2;
    const int center_y = rect_height / 2;
    //取最短边为边长为宽度（3*2个圆）
    const int item_width = ((this->width() / 3 < this->height() / 2)
                            ? this->width() / 3
                            : this->height() / 2) - 10;
    //item在rect的位置
    const int item_left = (rect_width - item_width) / 2;
    const int item_top = (rect_height - item_width) / 2;
    //(一个圆加一个方框)
    QPainterPath path;
    path.addRect(item_left, item_top, item_width, item_width);
    //path.addEllipse(item_left, item_top, item_width-10, item_width-10);
    path.addEllipse(QPoint(rect_width / 2, rect_height / 2),
                    item_width / 2 - 5, item_width / 2 - 5);

    //依次试用pen style枚举值
    pen.setColor(Qt::darkBlue); //原谅色
    //（如果线宽是奇数，直线抗锯齿会模糊，这是Qt的bug）
    pen.setWidth(4);  //线宽
    //不开抗锯齿曲线有锯齿
    painter.setRenderHint(QPainter::Antialiasing);
    //保存位置，等会儿换行用restore恢复，然后下移画第二行
    painter.save();

    //Qt::NoPen 啥都没有
    //Qt::SolidLine 实线，默认值
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);
    painter.drawPath(path);
    painter.drawText(item_left + 10, item_top + 20, "SolidLine");

    //Qt::DashLine 虚线
    pen.setStyle(Qt::DashLine);
    painter.setPen(pen);
    painter.translate(rect_width, 0); //右移
    painter.drawPath(path);
    painter.drawText(item_left + 10, item_top + 20, "DashLine");

    //Qt::DotLine 点线
    pen.setStyle(Qt::DotLine);
    painter.setPen(pen);
    painter.translate(rect_width, 0); //右移
    painter.drawPath(path);
    painter.drawText(item_left + 10, item_top + 20, "DotLine");

    //恢复位置后画准备第二行
    painter.restore();
    painter.save();

    //Qt::DashDotLine 混合
    pen.setStyle(Qt::DashDotLine);
    painter.setPen(pen);
    painter.translate(0, rect_height); //下移
    painter.drawPath(path);
    painter.drawText(item_left + 10, item_top + 20, "DashDotLine");

    //Qt::DashDotDotLine 混合
    pen.setStyle(Qt::DashDotDotLine);
    painter.setPen(pen);
    painter.translate(rect_width, 0); //右移
    painter.drawPath(path);
    painter.drawText(item_left + 10, item_top + 20, "DashDotDotLine");

    //Qt::CustomDashLine
    //除了要设置pen style为Qt::CustomDashLine外，
    //还需要调用setDashPattern来描述自定义虚线的样子
    pen.setStyle(Qt::CustomDashLine);
    //参数奇数为线长度，偶数为线间隔（绘制的时候他好像没把线宽考虑进去）
    pen.setDashPattern(customDash);
    //定时移动偏移，蚂蚁线效果
    pen.setDashOffset(dashOffset);
    painter.setPen(pen);
    painter.translate(rect_width, 0); //右移
    painter.drawPath(path);
    painter.drawText(item_left + 10, item_top + 20, "CustomDashLine");

    //恢复位置准备画cap和join示意
    painter.restore();
    const int line_width = 10;
    path = QPainterPath(); //低版本没有clear
    path.moveTo(center_x - line_width * 4, center_y - line_width * 2);
    path.lineTo(center_x - line_width * 1, center_y - line_width * 2);
    path.lineTo(center_x - line_width * 1, center_y);
    path.lineTo(center_x - line_width * 4, center_y);
    path.lineTo(center_x - line_width * 4, center_y + line_width * 2);
    path.lineTo(center_x - line_width * 1, center_y + line_width * 2);
    path.moveTo(center_x + line_width * 4, center_y);
    path.lineTo(center_x + line_width * 1, center_y);
    path.lineTo(center_x + line_width * 2.5, center_y - line_width * 2);
    path.lineTo(center_x + line_width * 2.5, center_y + line_width * 2);
    pen.setStyle(Qt::SolidLine); //默认值
    pen.setCapStyle(Qt::SquareCap); //默认值
    pen.setJoinStyle(Qt::BevelJoin); //默认值
    pen.setColor(Qt::red);
    pen.setWidth(line_width);
    //保存位置，等会儿换行用restore恢复，然后下移画第二行
    painter.save();

    //第一行CapStyle端点样式
    pen.setCapStyle(Qt::SquareCap);
    painter.setPen(pen);
    painter.drawPath(path);
    painter.drawText(center_x - 20, center_y + line_width * 4, "SquareCap");

    pen.setCapStyle(Qt::FlatCap);
    painter.setPen(pen);
    painter.translate(rect_width, 0); //右移
    painter.drawPath(path);
    painter.drawText(center_x - 20, center_y + line_width * 4, "FlatCap");

    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);
    painter.translate(rect_width, 0); //右移
    painter.drawPath(path);
    painter.drawText(center_x - 20, center_y + line_width * 4, "RoundCap");

    //恢复位置后画准备第二行
    painter.restore();
    pen.setStyle(Qt::SolidLine); //默认值
    pen.setCapStyle(Qt::SquareCap); //默认值
    pen.setJoinStyle(Qt::BevelJoin); //默认值
    pen.setColor(Qt::green);
    painter.save();

    //第二行画JoinStyle交点样式
    pen.setJoinStyle(Qt::BevelJoin);
    painter.setPen(pen);
    painter.translate(0, rect_height); //下移
    painter.drawPath(path);
    painter.drawText(center_x - 20, center_y + line_width * 4, "BevelJoin");

    pen.setJoinStyle(Qt::MiterJoin);
    painter.setPen(pen);
    painter.translate(rect_width, 0); //右移
    painter.drawPath(path);
    painter.drawText(center_x - 20, center_y + line_width * 4, "MiterJoin");

    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);
    painter.translate(rect_width, 0); //右移
    painter.drawPath(path);
    painter.drawText(center_x - 20, center_y + line_width * 4, "RoundJoin");
    painter.restore();
}
