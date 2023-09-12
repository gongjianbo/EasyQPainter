#pragma once
#include <QWidget>
#include <QImage>

// 颜色混合方式
// Qt 示例中有个比较有参考性的例子：Composition
class BasicComposition : public QWidget
{
    Q_OBJECT
public:
    // 对应 QPainter::CompositionMode
    // 源码实现：qt-everywhere-src-5.15.2\qtbase\src\gui\painting\qdrawhelper_mips_dsp.cpp
    enum QtComposition {
        // 这是默认模式，源的 alpha 用于混合目标顶部的像素
        SourceOver,
        // 目标的 alpha 用于将其混合到源像素之上，此模式与 SourceOver 相反
        DestinationOver,
        // 清除，设置为完全透明
        Clear,
        // 输出是源像素，这意味着基本的复制操作，当源像素不透明时，与 SourceOver 相同
        Source,
        // 输出是目标像素，这意味着混合没有效果，此模式与 Source 相反
        Destination,
        // 输出是源，其中 alpha 减少目标的 alpha
        SourceIn,
        // 输出是目标，其中 alpha 值被源的 alpha 值减少，此模式与 SourceIn 相反
        DestinationIn,
        // 输出是源，其中 alpha 被目标的倒数减小
        SourceOut,
        // 输出是目标，其中 alpha 被源的倒数减小，此模式与 SourceOut 相反
        DestinationOut,
        // 源像素在目标像素的顶部混合，源像素的 alpha 值减去目标像素的 alpha
        SourceAtop,
        // 目标像素在源之上混合，目标像素的 Alpha 会减小目标像素的 Alpha，此模式与 SourceAtop 相反
        DestinationAtop,
        // 源的 alpha 随目标 alpha 的倒数而减小，与目标合并，其 alpha 由源 alpha 的倒数减小，此 Xor 与按位 Xor 不同。
        Xor,

        // svg 1.2 blend modes

        // 源像素和目标像素的 alpha 像素和颜色相加
        Plus,
        // 输出是源颜色乘以目标，将颜色与白色相乘会使颜色保持不变，而将颜色与黑色相乘会产生黑色
        Multiply,
        // 源颜色和目标颜色将反转，然后相乘，用白色筛选颜色会产生白色，而用黑色筛选颜色会保留颜色不变
        Screen,
        // 根据目标颜色乘以或屏蔽颜色，目标颜色与源颜色混合，以反映目标的明暗
        Overlay,
        // 选择源颜色和目标颜色的较深色
        Darken,
        // 选择源颜色和目标颜色的较浅色
        Lighten,
        // 目标颜色变亮以反映源颜色，黑色源颜色使目标颜色保持不变
        ColorDodge,
        // 目标颜色变暗以反映源颜色，白色源颜色使目标颜色保持不变
        ColorBurn,
        // 根据源颜色乘以或屏蔽颜色，光源颜色将使目标颜色变亮，而深光源颜色将使目标颜色变暗
        HardLight,
        // 根据源颜色使颜色变暗或变亮，类似于 HardLight
        SoftLight,
        // 从较浅的颜色中减去较深的颜色，用白色绘画会反转目标颜色，而用黑色绘画会使目标颜色保持不变
        Difference,
        // 与 Difference 相似，但对比度较低，用白色绘画会反转目标颜色，而用黑色绘画会使目标颜色保持不变
        Exclusion
    };
    Q_ENUM(QtComposition)
public:
    explicit BasicComposition(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    // 因为半透明效果不好在 QWidget 上展示，所以用一个 QImage 当画布，最后再画到界面
    QImage canvas;
    // Composition 的 destination 图案
    QImage dest;
    // Composition 的 source 用一个圆形的色块
    // 色块的位置
    QPoint circlePos{0, 0};
    // 色块的半径
    int circleRadius{100};
    // 色块透明度[0,255]
    int circleAlpha{255};
    // 色块颜色HSV的hue[0,359]，saturation和value固定255
    int circleHue{0};
    // 当前混合方式选择
    int circleComposition{SourceOver};
    // 是否在拖拽
    bool dragFlag{false};
    // 拖拽相对位置
    QPoint dragDiff{0, 0};
};
