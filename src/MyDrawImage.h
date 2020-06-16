#ifndef MYDRAWIMAGE_H
#define MYDRAWIMAGE_H

#include <QWidget>
#include <QImage>
#include <QComboBox>

//展示图片的绘制
class MyDrawImage : public QWidget
{
    Q_OBJECT
public:
    explicit MyDrawImage(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    //有两张图
    QImage imgA;
    QImage imgB;
};

#endif // MYDRAWIMAGE_H
