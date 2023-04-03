#include "LedLattice.h"
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QGradient>
#include <QBrush>
#include <QDebug>

// 爱心的字模 16*16 逐列，灯亮为 1，左上角为低位取行列 (0,0)
unsigned short led_data[] = {
    0b0000000000000000,
    0b0000000011100000,
    0b0000000111110000,
    0b0000001111111000,
    0b0000011111111000,
    0b0000111111111000,
    0b0001111111110000,
    0b0011111111100000,
    0b0011111111100000,
    0b0001111111110000,
    0b0000111111111000,
    0b0000011111111000,
    0b0000001111111000,
    0b0000000111110000,
    0b0000000011100000,
    0b0000000000000000,
};

LedLattice::LedLattice(QWidget *parent)
    : QWidget(parent)
{
    connect(&timer, &QTimer::timeout, this, [this]
    {
        currentCol++;
        if(currentCol > colCount){
            currentCol = 0;
            // 扫描完一轮后图形移动一次
            colOffset = (colOffset + 1) % colCount;
        }
        update();
    });
}

void LedLattice::showEvent(QShowEvent *event)
{
    timer.start(100);
    QWidget::showEvent(event);
}

void LedLattice::hideEvent(QHideEvent *event)
{
    timer.stop();
    QWidget::hideEvent(event);
}

void LedLattice::paintEvent(QPaintEvent *event)
{
    event->accept();
    // 线高低电平的颜色
    const QColor high_color = QColor(255, 0, 0);
    const QColor low_color = QColor(0, 0, 255);
    const QColor led_color = QColor(0, 255, 0);
    const int line_width = 4;
    const int line_space = 16;
    const int led_width = 18;
    const int row_count = 16;
    const int col_count = 16;
    // 绘制范围
    const int area_width = col_count * line_width + (col_count + 1) * line_space;
    const int area_height = row_count * line_width + (row_count + 1) * line_space;
    QRect area = QRect(0, 0, area_width, area_height);
    area.moveCenter(rect().center());
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);
    painter.translate(area.topLeft());
    // 假设行为发光二极管正极，列为负极，列扫描
    // 默认列高电平行低电平，当前扫描到的列给低电平则该列高电平的行发光
    // 这里加 offset 用于实现移动效果
    unsigned short current_col_data = led_data[(currentCol + colOffset) % colCount];
    unsigned short current_row_index = 0x01;
    for (int row = 0; row < row_count; row++)
    {
        int row_offset = row * (line_width + line_space) + line_space;
        painter.setPen(QPen(low_color, line_width));
        painter.setOpacity(0.4);
        painter.drawLine(QPoint(0, row_offset), QPoint(area_width, row_offset));
        // 标出高电平引脚
        if (current_col_data & current_row_index)
        {
            painter.setPen(QPen(high_color, line_width));
            painter.setOpacity(1);
            painter.drawPoint(0, row_offset);
        }
        current_row_index <<= 1;
    }
    for (int col = 0; col < col_count; col++)
    {
        int col_offset = col * (line_width + line_space) + line_space;
        painter.setPen(QPen(high_color, line_width));
        painter.setOpacity(0.4);
        painter.drawLine(QPoint(col_offset, 0), QPoint(col_offset, area_height));
        // 当前扫描的列，标出低电平引脚
        if (col == currentCol)
        {
            painter.setPen(QPen(low_color, line_width));
            painter.setOpacity(1);
            painter.drawPoint(col_offset, 0);
        }
    }
    painter.setPen(QPen(led_color, led_width));
    for (int col = 0; col < col_count; col++)
    {
        int distance = 0;
        int offset = colOffset;
        if (currentCol >= col)
        {
            distance = currentCol - col;
        }
        else
        {
            // 后面的图形列保持之前的位置，不然会跳一格
            offset = (colOffset + colCount - 1) % colCount;
            distance = currentCol + colCount - col;
        }
        // 模拟荧光效果，当前扫描的列亮度为 1，逐渐变暗
        painter.setOpacity((colCount - distance) / (double)colCount);
        //
        int col_offset = col * (line_width + line_space) + line_space;
        unsigned short col_data = led_data[(col + offset) % colCount];
        unsigned short row_index = 0x01;
        for (int row = 0; row < row_count; row++)
        {
            int row_offset = row * (line_width + line_space) + line_space;
            if (col_data & row_index)
            {
                painter.drawPoint(col_offset, row_offset);
            }
            row_index <<= 1;
        }
    }
    painter.setOpacity(1);
}
