#include "BasicComposition.h"
#include <QComboBox>
#include <QSlider>
#include <QLabel>
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QtMath>
#include <QMetaEnum>
#include <QLinearGradient>
#include <QDebug>
#include <cmath>

// 和 GL 枚举的对应：qt-everywhere-src-5.15.2\qtbase\src\gui\opengl\qopenglpaintengine.cpp
#if 0
switch(q->state()->composition_mode) {
case QPainter::CompositionMode_SourceOver:
    funcs.glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    break;
case QPainter::CompositionMode_DestinationOver:
    funcs.glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
    break;
case QPainter::CompositionMode_Clear:
    funcs.glBlendFunc(GL_ZERO, GL_ZERO);
    break;
case QPainter::CompositionMode_Source:
    funcs.glBlendFunc(GL_ONE, GL_ZERO);
    break;
case QPainter::CompositionMode_Destination:
    funcs.glBlendFunc(GL_ZERO, GL_ONE);
    break;
case QPainter::CompositionMode_SourceIn:
    funcs.glBlendFunc(GL_DST_ALPHA, GL_ZERO);
    break;
case QPainter::CompositionMode_DestinationIn:
    funcs.glBlendFunc(GL_ZERO, GL_SRC_ALPHA);
    break;
case QPainter::CompositionMode_SourceOut:
    funcs.glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ZERO);
    break;
case QPainter::CompositionMode_DestinationOut:
    funcs.glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
    break;
case QPainter::CompositionMode_SourceAtop:
    funcs.glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    break;
case QPainter::CompositionMode_DestinationAtop:
    funcs.glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_SRC_ALPHA);
    break;
case QPainter::CompositionMode_Xor:
    funcs.glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    break;
case QPainter::CompositionMode_Plus:
    funcs.glBlendFunc(GL_ONE, GL_ONE);
    break;
case QPainter::CompositionMode_Multiply:
    funcs.glBlendEquation(GL_MULTIPLY_KHR);
    break;
case QPainter::CompositionMode_Screen:
    funcs.glBlendEquation(GL_SCREEN_KHR);
    break;
case QPainter::CompositionMode_Overlay:
    funcs.glBlendEquation(GL_OVERLAY_KHR);
    break;
case QPainter::CompositionMode_Darken:
    funcs.glBlendEquation(GL_DARKEN_KHR);
    break;
case QPainter::CompositionMode_Lighten:
    funcs.glBlendEquation(GL_LIGHTEN_KHR);
    break;
case QPainter::CompositionMode_ColorDodge:
    funcs.glBlendEquation(GL_COLORDODGE_KHR);
    break;
case QPainter::CompositionMode_ColorBurn:
    funcs.glBlendEquation(GL_COLORBURN_KHR);
    break;
case QPainter::CompositionMode_HardLight:
    funcs.glBlendEquation(GL_HARDLIGHT_KHR);
    break;
case QPainter::CompositionMode_SoftLight:
    funcs.glBlendEquation(GL_SOFTLIGHT_KHR);
    break;
case QPainter::CompositionMode_Difference:
    funcs.glBlendEquation(GL_DIFFERENCE_KHR);
    break;
case QPainter::CompositionMode_Exclusion:
    funcs.glBlendEquation(GL_EXCLUSION_KHR);
    break;
default:
    qWarning("Unsupported composition mode");
    break;
}
#endif

BasicComposition::BasicComposition(QWidget *parent)
    : QWidget(parent)
{
    // 下拉框选择混合方式
    QMetaEnum me = QMetaEnum::fromType<QtComposition>();
    QStringList items;
    for (int i = 0; i <= (int)Exclusion; i++)
    {
        items.push_back(me.valueToKey(i));
    }
    QComboBox *item_box = new QComboBox(this);
    item_box->addItems(items);
    item_box->move(100, 10);
    QLabel *item_lab = new QLabel("Composition", this);
    item_lab->move(10, 10);
    connect(item_box, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        circleComposition = index;
        update();
    });

    // 滑动条选择颜色
    QSlider *hue_slider = new QSlider(Qt::Horizontal, this);
    hue_slider->setRange(0, 359);
    hue_slider->setValue(circleHue);
    hue_slider->move(100, 50);
    QLabel *hue_lab = new QLabel("Color(HSV-Hue)", this);
    hue_lab->move(10, 50);
    connect(hue_slider, &QSlider::valueChanged, [=](int value) {
        circleHue = value;
        update();
    });

    // 滑动条选择透明度
    QSlider *alpha_slider = new QSlider(Qt::Horizontal, this);
    alpha_slider->setRange(0, 255);
    alpha_slider->setValue(circleAlpha);
    alpha_slider->move(100, 90);
    QLabel *alpha_lab = new QLabel("Color(Alpha)", this);
    alpha_lab->move(10, 90);
    connect(alpha_slider, &QSlider::valueChanged, [=](int value) {
        circleAlpha = value;
        update();
    });
}

void BasicComposition::paintEvent(QPaintEvent *event)
{
    event->accept();
    {
        QPainter p(this);
        // 灰白相间的格子，用于查看半透明
        int rect_size = 50;
        bool h_white = true;
        bool w_white = true;
        for (int h = 0; h < height(); h += rect_size)
        {
            w_white = h_white;
            for (int w = 0; w < width(); w += rect_size)
            {
                p.fillRect(w, h, rect_size, rect_size, w_white ? Qt::white : Qt::gray);
                w_white = !w_white;
            }
            h_white = !h_white;
        }
    }

    if (canvas.isNull() || dest.isNull())
        return;
    // 把图案画到 canvas，然后叠加 source 色块
    {
        // 如果用 QPainter 清除，需要 CompositionMode_Clear
        // 默认的 CompositionMode_SourceOver 会混合历史数据
        canvas.fill(Qt::transparent);
        QPainter p(&canvas);
        p.setPen(Qt::NoPen);
        p.drawImage(0, 0, dest);
        // 应用设置的 Comsposition
        p.setCompositionMode((QPainter::CompositionMode)circleComposition);
        // 色块
        p.setBrush(QColor::fromHsv(circleHue, 255, 255, circleAlpha));
        p.setRenderHint(QPainter::Antialiasing);
        p.drawEllipse(circlePos, circleRadius, circleRadius);
    }

    // 把 canvas 画到 widget
    {
        QPainter p(this);
        p.drawImage(0, 0, canvas);
    }
}

void BasicComposition::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (!event->size().isValid())
        return;
    canvas = QImage(event->size(), QImage::Format_ARGB32_Premultiplied);
    dest = QImage(event->size(), QImage::Format_ARGB32_Premultiplied);
    dest.fill(Qt::transparent);

    QPainter p(&dest);
    p.setPen(Qt::NoPen);
    const int w = dest.width();
    const int h = dest.height();

    // 右侧 - 渐变的色条
    // 竖向颜色渐变 + 横向半透明渐变，参考示例 composition
    p.save();
    QLinearGradient rect_gradient(0, 0, 0, h);
    rect_gradient.setColorAt(0, Qt::red);
    rect_gradient.setColorAt(.17, Qt::yellow);
    rect_gradient.setColorAt(.33, Qt::green);
    rect_gradient.setColorAt(.50, Qt::cyan);
    rect_gradient.setColorAt(.66, Qt::blue);
    rect_gradient.setColorAt(.81, Qt::magenta);
    rect_gradient.setColorAt(1, Qt::red);
    p.setBrush(rect_gradient);
    // 画在右边
    p.drawRect(w / 2, 0, w / 2, h);

    QLinearGradient alpha_gradient(0, 0, w, 0);
    alpha_gradient.setColorAt(0.2, Qt::white);
    alpha_gradient.setColorAt(0.5, Qt::transparent);
    alpha_gradient.setColorAt(0.8, Qt::white);

    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.setBrush(alpha_gradient);
    p.drawRect(0, 0, w, h);
    p.restore();

    // 左侧 - 渐变的刺球
    // 准备一个正方形区域
    p.save();
    QRect area;
    if (w > h) {
        area = QRect(0, 0, h, h);
    } else {
        area = QRect(0, (h - w) / 2, w, w);
    }
    int r = area.width() / 2 - 20;
    if (r < 50) {
        r = 50;
    }
    // 移动到正方形区域中心
    p.translate(area.center());
    // 准备渐变
    QLinearGradient r_gradient(0, 0, 0, r);
    r_gradient.setColorAt(0, QColor(0, 0, 255, 255));
    r_gradient.setColorAt(1, QColor(0, 255, 255, 127));
    p.setBrush(r_gradient);
    // 准备三角
    QPainterPath r_path;
    r_path.moveTo(0, r);
    r_path.lineTo(-20, 0);
    r_path.lineTo(20, 0);
    r_path.closeSubpath();
    const int r_angle = 18;
    p.setRenderHint(QPainter::Antialiasing);
    for (int i = 0; i < 360; i += r_angle)
    {
        p.drawPath(r_path);
        p.rotate(r_angle);
    }
    p.restore();
}

void BasicComposition::mousePressEvent(QMouseEvent *event)
{
    event->accept();
    QPoint pos = event->pos();
    // 右键点击重置位置
    if (event->button() & Qt::RightButton) {
        circlePos = pos;
        update();
        return;
    }
    QPoint diff = circlePos - pos;
    // 距离圆心小于半径就拖动
    if (std::hypot(diff.x(), diff.y()) <= circleRadius) {
        dragDiff = diff;
        dragFlag = true;
    }
}

void BasicComposition::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();
    if (dragFlag) {
        circlePos = dragDiff + event->pos();
        update();
    }
}

void BasicComposition::mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();
    if (dragFlag) {
        circlePos = dragDiff + event->pos();
        // 超出范围重置
        if (!canvas.rect().contains(circlePos)) {
            circlePos = QPoint(0, 0);
        }
        update();
    }
    dragFlag = false;
}
