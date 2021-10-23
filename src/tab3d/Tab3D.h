#pragma once
#include <QWidget>

namespace Ui {
class Tab3D;
}

class Tab3D : public QWidget
{
    Q_OBJECT

public:
    explicit Tab3D(QWidget *parent = nullptr);
    ~Tab3D();

private:
    Ui::Tab3D *ui;
};
