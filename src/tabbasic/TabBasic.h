#pragma once
#include <QWidget>

namespace Ui
{
    class TabBasic;
}

//一些基本的接口效果演示
class TabBasic : public QWidget
{
    Q_OBJECT

public:
    explicit TabBasic(QWidget *parent = nullptr);
    ~TabBasic();

private:
    Ui::TabBasic *ui;
};
