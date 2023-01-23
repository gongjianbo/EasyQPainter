#pragma once
#include <QWidget>

namespace Ui {
class TabDraw;
}

//绘制自定义效果
class TabDraw : public QWidget
{
    Q_OBJECT
public:
    explicit TabDraw(QWidget *parent = nullptr);
    ~TabDraw();

private:
    Ui::TabDraw *ui;
};

