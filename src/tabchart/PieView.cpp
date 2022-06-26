#include "PieView.h"
#include <algorithm>
#include <cmath>
#include <QtMath>
#include <QTime>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>

PieView::PieView(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true);

    //添加测试数据
    appendSlice(PieSlice("1 gong", 10));
    appendSlice(PieSlice("2 jian", 5));
    appendSlice(PieSlice("3 bo", 13));
    appendSlice(PieSlice("4 1992", 2));
}

PieView::~PieView()
{

}

void PieView::appendSlice(const PieSlice &slice)
{
    //去重
    for (const PieSlice &item : sliceList)
    {
        if(item.name == slice.name)
            return;
    }
    sliceList.append(slice);
    sliceValueCount += slice.value;
    //重新排序-降序
    std::sort(sliceList.begin(), sliceList.end(),
              [](const PieSlice &left, const PieSlice &right)->bool{
        return left.value > right.value;
    });
    double start_angle = 0; //起始角度temp，用于累加
    int h_value = -1000; //色度temp，用于相近色计算
    for (PieSlice &item : sliceList) {
        item.percentage = item.value / sliceValueCount; //计算百分比
        item.startAngle = start_angle; //起始角
        item.angleSpan = item.percentage * 360; //占的角度值
        start_angle += item.angleSpan;
        //此处用的随机颜色，也可以通过传入一个颜色列表来取对应颜色
        int new_h_value = std::rand() % 360;
        //本来想算一个不相近的颜色，但是0附近和359附近颜色也接近
        //并且，没有考虑整体的颜色独立性
        while(qAbs(new_h_value - h_value) < 60){
            new_h_value = std::rand() % 360;
        }
        //Hue 色度[0,359], Lightness 亮度[0,255], Saturation 饱和度[0,255]
        item.color = QColor::fromHsl(new_h_value, 220, 80);
        h_value = new_h_value; //用于下次计算色度相近
    }
    //刷新ui
    hoveredFlag = false;
    hoveredIndex = -1;
    update();
}

void PieView::clearSlice()
{
    sliceList.clear();
    sliceValueCount = 0.0;
    //刷新ui
    hoveredFlag = false;
    hoveredIndex = -1;
    update();
}

void PieView::paintEvent(QPaintEvent *event)
{
    event->accept();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    //绘制扇形
    painter.save();
    painter.translate(rect().center()); //中心点移动到pie的中心点
    painter.setPen(QColor(255, 255, 255));
    int index = 0;
    for (const PieSlice &item : sliceList)
    {
        //半径随index递减样式
        int slice_radius = (pieRadius - index * 5);
        //hover选中时半径突出一点
        if (hoveredFlag && index == hoveredIndex) {
            slice_radius += 10;
        }
        //slice扇形路径
        {
            QPainterPath path;
            path.moveTo(QPointF(0, 0));
            const QRectF pie_rect = QRectF(-slice_radius, -slice_radius,
                                           slice_radius * 2, slice_radius * 2);
            path.arcTo(pie_rect, item.startAngle, item.angleSpan);
            path.lineTo(QPointF(0, 0));
            painter.fillPath(path, QBrush(item.color.lighter()));
        }
        if (slice_radius > 20) {
            QPainterPath path;
            path.moveTo(QPointF(0, 0));
            const QRectF pie_rect = QRectF(-(slice_radius - 20), -(slice_radius - 20),
                                           (slice_radius - 20) * 2, (slice_radius - 20) * 2);
            path.arcTo(pie_rect, item.startAngle, item.angleSpan);
            path.lineTo(QPointF(0, 0));
            painter.fillPath(path, QBrush(item.color));
        }
        //根据扇形中心点绘制文本
        const QString text_percent = QString::number(item.percentage * 100, 'f', 2) + "%";
        const double text_angle = item.startAngle + item.angleSpan / 2; //span中心
        const int text_height = painter.fontMetrics().height() + 2; //加行间隔2
        const int text_namewidth = painter.fontMetrics().boundingRect(item.name).width(); //名称str宽度
        const int text_percentwidth = painter.fontMetrics().boundingRect(text_percent).width(); //值str宽度
        const double text_x = slice_radius * 0.6 * std::cos(text_angle / 180 * M_PI); //文本中心点
        const double text_y = -slice_radius * 0.6 * std::sin(text_angle / 180 * M_PI); //文本中心点

        //y轴是上负下正，所以加减操作反过来了
        painter.drawText(text_x - text_namewidth / 2, text_y, item.name);
        painter.drawText(text_x - text_percentwidth / 2, text_y + text_height, text_percent);
        ++index;
    }
    painter.restore();

    //绘制hover选中slice的tip
    if (hoveredFlag && sliceList.count() > hoveredIndex && 0 <= hoveredIndex) {
        const int rect_margin = 5; //矩形边距
        const PieSlice &item = sliceList.at(hoveredIndex);
        const QString str_name = QString("name:%1").arg(item.name);
        const int name_width = painter.fontMetrics().boundingRect(str_name).width() + rect_margin * 2;
        const QString str_value = QString("value:%1(%2%)")
                .arg(QString::number(item.value, 'f', 0))
                .arg(QString::number(item.percentage * 100, 'f', 2));
        const int text_height = painter.fontMetrics().height();
        const int value_width = painter.fontMetrics().boundingRect(str_value).width() + rect_margin * 2;
        const int rect_height = text_height * 2 + rect_margin * 2 + 2; //两行+间隔2
        const int rect_width = name_width > value_width ? name_width : value_width;
        //左上角坐标，避免超出范围所以要判断并set
        QPointF top_left(mousePos.x() - rect_width, mousePos.y() - rect_height);
        if(top_left.x() < pieRect.x())
            top_left.setX(pieRect.x());
        if(top_left.y() < pieRect.y())
            top_left.setY(pieRect.y());
        //半透明矩形背景，可以fillpath绘制圆角矩形
        painter.fillRect(QRectF(top_left.x(), top_left.y(), rect_width, rect_height),
                         QBrush(QColor(150, 150, 150, 120)));
        painter.setPen(QColor(255, 255, 255));//绘制文本，这里没有设置字体，请自行设置
        painter.drawText(top_left.x() + rect_margin,
                         top_left.y() + rect_margin + text_height,
                         str_name);
        painter.drawText(top_left.x() + rect_margin,
                         top_left.y() + rect_margin + text_height * 2 + 2,
                         str_value);
    }
}

void PieView::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    pieRadius = (width() > height() ? height() : width()) / 2 - pieMargin;
    QRect pie_rect = QRect(0, 0, pieRadius * 2, pieRadius * 2);
    pie_rect.moveCenter(rect().center());
    pieRect = pie_rect;
}

void PieView::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
    mousePos = event->pos();
    //不在范围内则清除hover标志
    if (!pieRect.contains(mousePos)) {
        hoveredFlag = false;
        hoveredIndex = -1;
        update();
        return;
    }

    //计算当前所在角度
    const double arc_tan = qAtan2(mousePos.y() - pieRect.center().y(),
                                  mousePos.x() - pieRect.center().x());
    //aten2结果是以右侧为0点，顺时针半圆为正，逆时针半圆为负，单位是弧度？
    //需要转换为值正北为0点，顺时针增长，单位转为角度
    double arc_pos = arc_tan * 180 / M_PI;
    if (arc_pos < 0) {
        arc_pos = -arc_pos;
    } else if (arc_pos > 0) {
        arc_pos = 360 - arc_pos;
    }

    //计算hover选中的index
    int index = 0;
    for (const PieSlice &item : sliceList)
    {
        if (arc_pos >= item.startAngle && arc_pos <= (item.startAngle + item.angleSpan)) {
            if (index != hoveredIndex) {
                hoveredIndex = index;
            }
            break;
        }
        ++index;
    }
    //因为由一个tip跟随鼠标移动，所以每次move都update
    hoveredFlag = true;
    update();
}
