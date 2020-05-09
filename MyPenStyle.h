#ifndef MYPENSTYLE_H
#define MYPENSTYLE_H

#include <QWidget>

class MyPenStyle : public QWidget
{
    Q_OBJECT
public:
    explicit MyPenStyle(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // MYPENSTYLE_H
