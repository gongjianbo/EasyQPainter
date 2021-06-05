#include "MyTextPath.h"

#include <QPaintEvent>
#include <QResizeEvent>
#include <QtMath>
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QTimer>
#include <QDebug>

MyTextPath::MyTextPath(QWidget *parent)
    : QWidget(parent)
{
    //定时移动
    QTimer *timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,[=](){
        //左侧文字区域宽高变化
        if(areaAdd){
            areaWidth+=2;
            if(areaWidth>400){
                areaAdd=false;
                areaWidth=400;
            }
        }else{
            areaWidth-=2;
            if(areaWidth<100){
                areaAdd=true;
                areaWidth=100;
            }
        }
        //底部文本从左往右循环滚动
        textOffset_1+=3;
        if (textOffset_1 > textWidth_1 + labelWidth_1) {
            textOffset_1 = 0;
        }
        //底部文本从右往左循环滚动
        textOffset_2+=3;
        if (textOffset_2 > textWidth_2 + labelWidth_2) {
            textOffset_2 = 0;
        }

        if(isHidden())
            return;
        update();
    });
    timer->start(50);
}

void MyTextPath::paintEvent(QPaintEvent *event)
{
    event->accept();
    QPainter painter(this);
    //先画一个白底黑框
    painter.fillRect(this->rect(),Qt::white);
    QPen pen(Qt::black);
    painter.setPen(pen);
    painter.drawRect(this->rect().adjusted(0,0,-1,-1)); //右下角会超出范围
    //窗口最短边
    const int border_width=width()>height()?height():width();

    //1.中心绘制一个Qt文字
    QColor qt_color(200,250,200); //淡淡的原谅色
    QFont qt_font("Microsoft YaHei",border_width/3);
    QPen qt_pen(qt_color,border_width/20+1);
    painter.setFont(qt_font);
    painter.setPen(qt_pen);
    const QString qt_str="Qt";
    //文本的pos是在左下角
    const int qt_str_width=painter.fontMetrics().width(qt_str);
    const int qt_str_height=painter.fontMetrics().height();
    //ascent从基线到字符延伸到的最高位置的距离
    //descent从基线到最低点字符延伸到的距离
    const int qt_str_descent=painter.fontMetrics().descent();
    const int qt_left=(width()-qt_str_width)/2;
    const int qt_bottom=(height()+qt_str_height)/2;
    const int qt_bottom_real=(height()+qt_str_height)/2-qt_str_descent;
    const QRect qt_rect=QRect(QPoint(qt_left,qt_bottom-qt_str_height),
                              QPoint(qt_left+qt_str_width,qt_bottom));
    //绘制文字的时候，如果使用rect指定位置，那么就在矩形中间绘制，可以指定位置flags
    //如果使用xy指定位置，会受到基线等影响，上下没对齐，要计算基线的偏移
    //painter.drawText(qt_rect,qt_str);
    //painter.drawText(qt_left,qt_bottom_real,qt_str);
    QPainterPath qt_str_path;
    qt_str_path.addText(qt_left,qt_bottom_real,qt_font,qt_str);
    //画路径
    painter.setPen(QPen(qt_color,2,Qt::DashLine));
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawPath(qt_str_path);
    painter.drawRect(qt_rect);

    //2.左上角绘制文字
    const QString str_1="一切都是没有结局的开始";
    const QString str_2="一切都是稍纵即逝的追寻";
    const QFont str_font("Microsoft YaHei",28);
    const int str_height=QFontMetrics(str_font).height();
    QPainterPath str_path;
    //暂时用的固定值坐标
    str_path.addText(20,str_height,str_font,str_1);
    str_path.addText(20,str_height*2,str_font,str_2);
    painter.setPen(QPen(Qt::green,0.5));
    QLinearGradient str_gradient(0,0,0,str_height*2);
    //用渐变色来实现文字填充一半的效果，中间部分填充透明色
    //百分比用值计算好点，我这里手动调的固定值
    str_gradient.setColorAt(0.0,Qt::green);
    str_gradient.setColorAt(0.35,Qt::green);
    str_gradient.setColorAt(0.351,Qt::transparent);
    str_gradient.setColorAt(0.85,Qt::transparent);
    str_gradient.setColorAt(0.851,Qt::green);
    str_gradient.setColorAt(1.0,Qt::green);
    painter.drawPath(str_path);
    painter.fillPath(str_path,str_gradient);

    //3.左侧文字
    QRect left_area(0,0,areaWidth,420-areaWidth);
    left_area.moveTo(20,((height()-left_area.height())/2));
    painter.setPen(QPen(QColor(255,0,0),2));
    painter.drawRect(left_area);
    QFont ft;
    ft.setFamily("SimSun");
    ft.setPixelSize(left_area.height());
    ft.setStretch(100);
    QFontMetrics fm(ft);
    const QString text="Hello!龚建波1992";
    double scale=left_area.width()/double(fm.width(text)+0.1)*100;
    //最小拉伸因子为 1，最大拉伸因子为 4000
    ft.setStretch(scale);
    QFontMetrics fm2(ft);
    if(prevWidth>0){
        //拉伸之后Qt计算可能有点问题
        //有些字号更大了，但是拉伸后反而变窄了
        //所以保持上次的字体大小和宽度，异常则使用上次的字体
        //使之增加时不能小于上次，减小时不能大于上次
        if((areaAdd && fm2.width(text)<prevWidth)||
                (!areaAdd && fm2.width(text)>prevWidth))
        {
            ft.setPixelSize(prevSize);
            ft.setStretch(prevStretch);
        }
    }
    //qDebug()<<scale<<fm2.width(text)<<ft.stretch()<<fm.width(text);
    painter.setFont(ft);
    //drawText y是基线位置，ascent 是基线到文字top的位置
    painter.drawText(left_area.left()+qCeil((left_area.width()-painter.fontMetrics().width(text))/2),
                     left_area.top()+painter.fontMetrics().ascent(),
                     text);
    prevWidth=painter.fontMetrics().width(text);
    prevSize=ft.pixelSize();
    prevStretch=ft.stretch();

    //4.底部滚动的文字1
    QFont scroll_font("Microsoft YaHei",20);
    painter.setFont(scroll_font);
    const QString scroll_text0="你一会儿看我，一会儿看云。";
    QString scroll_text1;
    for(auto riter=scroll_text0.rbegin();riter!=scroll_text0.rend();riter++)
        scroll_text1.push_back(*riter);
    const int scroll_width1 = painter.fontMetrics().width(scroll_text1);
    const int scroll_height1 = painter.fontMetrics().height();
    const int scroll_y1 = height()-scroll_height1-10; //Qt文本起点在左下角
    //文本宽度改变，重新滚动(包括设置了文本，修改了字体大小等)
    if (textWidth_1 != scroll_width1 && scroll_width1 > 0) {
        textWidth_1 = scroll_width1;
        textOffset_1 = 0;
    }
    else {
        //彩色文本
        QLinearGradient gradient(textOffset_1-textWidth_1,0,
                                 textOffset_1,0);
        gradient.setColorAt(0,QColor(Qt::red));
        gradient.setColorAt(0.33,QColor(Qt::yellow));
        gradient.setColorAt(0.66,QColor(Qt::green));
        gradient.setColorAt(1,QColor(Qt::blue));
        painter.setPen(QPen(QBrush(gradient),2));
        painter.drawText(textOffset_1-textWidth_1, scroll_y1, scroll_text1);
    }

    //5.底部滚动的文字2
    const QString scroll_text2="我觉得你看我时很远，你看云时很近。";
    const int scroll_width2 = painter.fontMetrics().width(scroll_text2);
    //const int scroll_height2 = painter.fontMetrics().capHeight();
    const int scroll_y2 = height()-10; //Qt文本起点在左下角
    //文本宽度改变，重新滚动(包括设置了文本，修改了字体大小等)
    if (textWidth_2 != scroll_width2 && scroll_width2 > 0) {
        textWidth_2 = scroll_width2;
        textOffset_2 = 0;
    }
    else {
        //彩色文本
        QLinearGradient gradient(labelWidth_2-textOffset_2,0,
                                 labelWidth_2-textOffset_2+textWidth_2,0);
        gradient.setColorAt(0,QColor(Qt::red));
        gradient.setColorAt(0.33,QColor(Qt::yellow));
        gradient.setColorAt(0.66,QColor(Qt::green));
        gradient.setColorAt(1,QColor(Qt::blue));
        painter.setPen(QPen(QBrush(gradient),2));
        painter.drawText(labelWidth_2-textOffset_2, scroll_y2, scroll_text2);
    }
}

void MyTextPath::resizeEvent(QResizeEvent *event)
{
    const int old_width = event->oldSize().width();
    const int new_width = event->size().width();
    //从左往右，比之前的更小，重新滚动
    if (new_width > 10) {
        labelWidth_1 = new_width;
        if (new_width < old_width) {
            textOffset_1 = 0;
        }
    }
    //从右往左，比之前的更小，重新滚动
    if (new_width > 10) {
        labelWidth_2 = new_width;
        if (new_width < old_width) {
            textOffset_2 = 0;
        }
    }
    QWidget::resizeEvent(event);
}
