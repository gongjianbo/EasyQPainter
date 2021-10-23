#pragma once
#include <QWidget>

namespace Ui {
class TabDraw;
}

class TabDraw : public QWidget
{
    Q_OBJECT

public:
    explicit TabDraw(QWidget *parent = nullptr);
    ~TabDraw();

private:
    Ui::TabDraw *ui;
};

