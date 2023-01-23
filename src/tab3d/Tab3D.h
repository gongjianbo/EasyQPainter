#pragma once
#include <QWidget>

namespace Ui{
class Tab3D;
}

//简单的立体效果
class Tab3D : public QWidget
{
    Q_OBJECT
public:
    explicit Tab3D(QWidget *parent = nullptr);
    ~Tab3D();

private:
    Ui::Tab3D *ui;
};
