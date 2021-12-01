#pragma once
#include <QWidget>

//选区绘制和交互
class SimpleSelection : public QWidget
{
    Q_OBJECT
public:
    //鼠标在区域的哪个位置
    enum AreaPosition : int
    {
        Outside = 0x00,
        Inside = 0xFF, //任意值
        AtLeft = 0x01,
        AtRight = 0x02,
        AtTop = 0x10,
        AtBottom = 0x20,
        AtTopLeft = 0x11, //AtLeft|AtTop
        AtTopRight = 0x12, //AtRight|AtTop
        AtBottomLeft = 0x21, //AtLeft|AtBottom
        AtBottomRight = 0x22 //AtRight|AtBottom
    };
    //当前编辑类型
    enum EditType : int
    {
        EditNone, //无操作
        PressInside, //在选区范围内按下
        PressOutside, //在选区范围外按下
        DrawSelection, //绘制
        MoveSelection, //拖动
        EditSelection //拉伸编辑
    };

public:
    explicit SimpleSelection(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    //计算鼠标相对区域的位置
    AreaPosition calcPosition(const QPoint &pos);
    //当前鼠标对应选区的位置
    void setCurPosition(AreaPosition position);
    //根据鼠标当前位置更新鼠标样式
    void updateCursor();

private:
    //当前选区
    //QRect有四个成员变量，分别对应左上角和右下角点坐标
    //x1-左上角坐标x
    //x2-等于x1+width-1
    //y1-左上角坐标y
    //y2-等于y1+height-1
    //即QRect(50,50,200,200)时，topLeft=(50,50)bottomRight=(249,249)
    //fillRect会填充整个区域
    //drawRect在画笔宽度奇数时，右下角会多1px，绘制时整体宽度先减去1px
    QRect selection;
    //是否有选区
    bool hasSelection{false};
    //鼠标当前操作位置
    AreaPosition curPosition{AreaPosition::Outside};
    //当前操作类型
    EditType curEditType{EditType::EditNone};
    //鼠标按下标志
    bool pressFlag{false};
    //鼠标按下位置
    QPoint pressPos;
    //目前用于记录press时鼠标与选区左上角的坐标差值
    QPoint tempPos;
    //鼠标当前位置
    QPoint mousePos;

    //最小宽度
    static const int Min_Width{5};
};
