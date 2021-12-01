#include "SimpleSelection.h"
#include <cmath>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QCursor>
#include <QDebug>

SimpleSelection::SimpleSelection(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true);

    selection = QRect(50, 50, 200, 200);
    hasSelection = true;
}

void SimpleSelection::paintEvent(QPaintEvent *event)
{
    event->accept();
    QPainter painter(this);
    //黑底
    painter.fillRect(this->rect(), Qt::black);

    if (!hasSelection)
        return;

    painter.save();
    if (pressFlag && curPosition != AreaPosition::Outside)
    { //点击
        painter.setPen(QColor(255, 0, 0));
        painter.setBrush(QColor(255, 170, 0));
    }
    else if (curPosition != AreaPosition::Outside)
    { //悬停
        painter.setPen(QColor(255, 0, 0));
        painter.setBrush(QColor(255, 200, 0));
    }
    else
    { //未选中
        painter.setPen(QColor(0, 170, 255));
        painter.setBrush(QColor(255, 200, 0));
    }

    painter.drawRect(selection.adjusted(0, 0, -1, -1));
    painter.restore();
}

void SimpleSelection::mousePressEvent(QMouseEvent *event)
{
    event->accept();
    mousePos = event->pos();
    if (event->button() == Qt::LeftButton)
    {
        pressFlag = true;
        pressPos = event->pos();
        if (curPosition == AreaPosition::Inside)
        {
            curEditType = PressInside;
            tempPos = mousePos - selection.topLeft();
        }
        else if (curPosition != AreaPosition::Outside)
        {
            curEditType = EditSelection;
        }
        else
        {
            curEditType = PressOutside;
        }
    }
    else
    {
        //非单独按左键
    }
    update();
}

void SimpleSelection::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();
    mousePos = event->pos();
    if (pressFlag)
    {
        if (curEditType == PressInside)
        {
            if (QPoint(pressPos - mousePos).manhattanLength() > 3)
            {
                curEditType = MoveSelection;
            }
        }
        else if (curEditType == PressOutside)
        {
            if (QPoint(pressPos - mousePos).manhattanLength() > 3)
            {
                curEditType = DrawSelection;
            }
        }

        QPoint mouse_p = mousePos;
        //限制范围在可视区域
        if (mouse_p.x() < 0)
        {
            mouse_p.setX(0);
        }
        else if (mouse_p.x() > width() - 1)
        {
            mouse_p.setX(width() - 1);
        }
        if (mouse_p.y() < 0)
        {
            mouse_p.setY(0);
        }
        else if (mouse_p.y() > height() - 1)
        {
            mouse_p.setY(height() - 1);
        }

        if (curEditType == DrawSelection)
        {
            hasSelection = true;
            selection = QRect(pressPos, mouse_p);
        }
        else if (curEditType == MoveSelection)
        {
            selection.moveTopLeft(mousePos - tempPos);
            //限制范围在可视区域
            if (selection.left() < 0)
            {
                selection.moveLeft(0);
            }
            else if (selection.right() > width() - 1)
            {
                selection.moveRight(width() - 1);
            }
            if (selection.top() < 0)
            {
                selection.moveTop(0);
            }
            else if (selection.bottom() > height() - 1)
            {
                selection.moveBottom(height() - 1);
            }
        }
        else if (curEditType == EditSelection)
        {
            int position = curPosition;
            if (position & AtLeft)
            {
                if (mouse_p.x() < selection.right())
                {
                    selection.setLeft(mouse_p.x());
                }
                else
                {
                    selection.setLeft(selection.right() - 1);
                }
            }
            else if (position & AtRight)
            {
                if (mouse_p.x() > selection.left())
                {
                    selection.setRight(mouse_p.x());
                }
                else
                {
                    selection.setRight(selection.left() + 1);
                }
            }
            if (position & AtTop)
            {
                if (mouse_p.y() < selection.bottom())
                {
                    selection.setTop(mouse_p.y());
                }
                else
                {
                    selection.setTop(selection.bottom() - 1);
                }
            }
            else if (position & AtBottom)
            {
                if (mouse_p.y() > selection.top())
                {
                    selection.setBottom(mouse_p.y());
                }
                else
                {
                    selection.setBottom(selection.top() + 1);
                }
            }
        }
    }
    else
    {
        setCurPosition(calcPosition(mousePos));
    }
    update();
}

void SimpleSelection::mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();
    mousePos = event->pos();
    pressFlag = false;
    if (curEditType != EditNone)
    {
        if (curEditType == DrawSelection)
        {
            selection = selection.normalized();
            if (selection.width() < Min_Width || selection.height() < Min_Width)
            {
                hasSelection = false;
            }
        }
        else if (curEditType == MoveSelection)
        {
        }
        else if (curEditType == EditSelection)
        {
            if (selection.width() < Min_Width || selection.height() < Min_Width)
            {
                hasSelection = false;
            }
        }
        curEditType = EditNone;
    }
    setCurPosition(calcPosition(mousePos));
    update();
}

SimpleSelection::AreaPosition SimpleSelection::calcPosition(const QPoint &pos)
{
    static const int check_radius = 3;
    int position = AreaPosition::Outside;
    if (!hasSelection)
    {
        return (SimpleSelection::AreaPosition)position;
    }
    //一条线太窄，不好触发，增加判断范围又会出现边界太近时交叠在一起
    //目前的策略是从右下开始判断，左上的优先级更低一点
    if (std::abs(pos.x() - selection.right()) < check_radius)
    {
        position |= AreaPosition::AtRight;
    }
    else if (std::abs(pos.x() - selection.left()) < check_radius)
    {
        position |= AreaPosition::AtLeft;
    }
    if (std::abs(pos.y() - selection.bottom()) < check_radius)
    {
        position |= AreaPosition::AtBottom;
    }
    else if (std::abs(pos.y() - selection.top()) < check_radius)
    {
        position |= AreaPosition::AtTop;
    }
    //没在边界上就判断是否在内部
    if (position == AreaPosition::Outside && selection.contains(pos))
    {
        position = AreaPosition::Inside;
    }
    return (SimpleSelection::AreaPosition)position;
}

void SimpleSelection::setCurPosition(AreaPosition position)
{
    if (position != curPosition)
    {
        curPosition = position;
        updateCursor();
    }
}

void SimpleSelection::updateCursor()
{
    switch (curPosition)
    {
    case AtLeft:
    case AtRight:
        setCursor(Qt::SizeHorCursor);
        break;
    case AtTop:
    case AtBottom:
        setCursor(Qt::SizeVerCursor);
        break;
    case AtTopLeft:
    case AtBottomRight:
        setCursor(Qt::SizeFDiagCursor);
        break;
    case AtTopRight:
    case AtBottomLeft:
        setCursor(Qt::SizeBDiagCursor);
        break;
    default:
        setCursor(Qt::ArrowCursor);
        break;
    }
}
