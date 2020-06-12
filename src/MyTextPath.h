#ifndef MYTEXTPATH_H
#define MYTEXTPATH_H

#include <QWidget>

//展示文字路径
class MyTextPath : public QWidget
{
    Q_OBJECT
public:
    explicit MyTextPath(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // MYTEXTPATH_H
