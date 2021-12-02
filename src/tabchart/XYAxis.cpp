#include "XYAxis.h"
#include <cmath>
#include <QtMath>
#include <QDebug>

XYAxis::XYAxis(AxisPosition position, QObject *parent)
    : QObject(parent), thePosition(position)
{

}

XYAxis::AxisPosition XYAxis::getAxisPosition() const
{
    return thePosition;
}

void XYAxis::setAxisPosition(AxisPosition position)
{
    if(thePosition != position){
        thePosition =position;
        emit axisChanged();
    }
}

XYAxis::TickMode XYAxis::getTickMode() const
{
    return theMode;
}

void XYAxis::setTickMode(TickMode mode)
{
    if(theMode!=mode){
        theMode=mode;
        calcAxis();
    }
}

QRect XYAxis::getRect() const
{
    return theRect;
}

void XYAxis::setRect(const QRect &rect)
{
    if(theRect != rect &&rect.isValid()){
        theRect = rect;
        calcAxis();
    }
}

int XYAxis::getDecimalPrecision() const
{
    return decimalPrecision;
}

void XYAxis::setDecimalPrecision(int precison)
{
    if(decimalPrecision != precison){
        decimalPrecision = precison;
        emit axisChanged();
    }
}

double XYAxis::getFixedValueSpace() const
{
    return fixedValueSpace;
}

void XYAxis::setFixedValueSpace(double value)
{
    fixedValueSpace = value;
    calcAxis();
}

int XYAxis::getRefPixelSpace() const
{
    return refPixelSpace;
}

void XYAxis::setRefPixelSpace(int pixel)
{
    refPixelSpace=pixel;
    calcAxis();
}

QVector<double> XYAxis::getTickPos() const
{
    return tickPos;
}

QVector<QString> XYAxis::getTickLabel() const
{
    return tickLabel;
}

double XYAxis::getMinLimit() const
{
    return minLimit;
}

void XYAxis::setMinLimit(double limit)
{
    minLimit = limit;
}

double XYAxis::getMaxLimit() const
{
    return maxLimit;
}

void XYAxis::setMaxLimit(double limit)
{
    maxLimit =limit;
}

double XYAxis::getMinRange() const
{
    return minRange;
}

void XYAxis::setMinRange(double limit)
{
    minRange =limit;
}

double XYAxis::getMinValue() const
{
    return minValue;
}

void XYAxis::setMinValue(double value)
{
    minValue=value;

}

double XYAxis::getMaxValue() const
{
    return maxValue;
}

void XYAxis::setMaxValue(double value)
{
    maxValue=value;
}

double XYAxis::getUnit1PxToValue() const
{
    return unit1PxToValue;
}

double XYAxis::getUnit1ValueToPx() const
{
    return unit1ValueToPx;
}

double XYAxis::pxToValue(double px) const
{
    return px*unit1PxToValue+minValue;
}

double XYAxis::valueToPx(double value) const
{
    return (value-minValue)*unit1ValueToPx;
}

void XYAxis::draw(QPainter *painter)
{
    painter->fillRect(theRect,Qt::green);
    switch (this->getAxisPosition()) {
    case AtRight:
        //drawRight(painter);
        break;
    case AtLeft:
        drawLeft(painter);
        break;
    case AtTop:
        //drawTop(painter);
        break;
    case AtBottom:
        drawBottom(painter);
        break;
    default:
        break;
    }
}

void XYAxis::drawLeft(QPainter *painter)
{
    painter->save();
    painter->drawLine(theRect.topRight(),theRect.bottomRight());

    const int right_pos=theRect.right();
    --todo应该从下往上计算
    for(int i=0;i<tickPos.count();i++){
        const int y_pos=std::floor(tickPos.at(i));
        painter->drawLine(QPoint(right_pos,y_pos),
                          QPoint(right_pos-5,y_pos));
        painter->drawText(right_pos-5-painter->fontMetrics().width(tickLabel.at(i)),
                          y_pos+painter->fontMetrics().height()/2,
                          tickLabel.at(i));
    }

    painter->restore();
}

void XYAxis::drawBottom(QPainter *painter)
{
    painter->save();
    painter->drawLine(theRect.topLeft(),theRect.topRight());

    const int top_pos=theRect.top();
    for(int i=0;i<tickPos.count();i++){
        const int x_pos=std::floor(tickPos.at(i));
        painter->drawLine(QPoint(x_pos,top_pos),
                          QPoint(x_pos,top_pos+5));
        painter->drawText(x_pos-painter->fontMetrics().width(tickLabel.at(i))/2,
                          top_pos+5+painter->fontMetrics().height(),
                          tickLabel.at(i));
    }
    painter->restore();
}

void XYAxis::calcAxis()
{
    if(minLimit>=maxLimit||theRect.isNull())
        return;
    if(minValue>maxValue){
        std::swap(minValue,maxValue);
    }
    if(minLimit>minValue){
        minValue=minLimit;
    }
    if(maxLimit<maxValue){
        maxValue=maxLimit;
    }
    switch (this->getAxisPosition()) {
    case AtBottom:
    {
        //横向x轴
        calcSpace(theRect.width());
        //计算刻度线
        const double right_pos=theRect.right();
        tickPos.clear();
        tickLabel.clear();
        const int precision=getTickPrecision();
        //i是用刻度px算坐标位置；j是用刻度px算i对应的value
        //条件i>pos-N是为了显示最大值那个刻度
        for(double i=theRect.left()+pxBegin,j=pxBegin;i<right_pos+2;i+=pxSpace,j+=pxSpace){
            tickPos.push_back(i);
            const double label_value=(minValue+(j)*unit1PxToValue);
            if(qFuzzyIsNull(label_value)){
                tickLabel.push_front("0");
            }else{
                const QString label_text=QString::number(label_value,'f',precision);
                tickLabel.push_back(label_text);
            }
        }
    }
        break;
    case AtLeft:
    {
        //竖向y轴
        calcSpace(theRect.height());
        //计算刻度线
        const double bottom_pos=theRect.bottom();
        tickPos.clear();
        tickLabel.clear();
        const int precision=getTickPrecision();
        //i是用刻度px算坐标位置；j是用刻度px算i对应的value
        //条件i>pos-N是为了显示最大值那个刻度
        for(double i=theRect.top()+pxBegin,j=pxBegin;i<bottom_pos+2;i+=pxSpace,j+=pxSpace){
            tickPos.push_front(i);
            const double label_value=(maxValue-(j)*unit1PxToValue);
            if(qFuzzyIsNull(label_value)){
                tickLabel.push_front("0");
            }else{
                const QString label_text=QString::number(label_value,'f',precision);
                tickLabel.push_front(label_text);
            }
        }
    }
        break;
    default:
        break;
    }
    emit axisChanged();
}

void XYAxis::calcSpace(double axisLength)
{
    //计算每单位值
    //为什么算了两个互为倒数的数呢？因为浮点数精度问题
    unit1PxToValue=(maxValue-minValue)/(axisLength);
    unit1ValueToPx=(axisLength)/(maxValue-minValue);
    //计算间隔和起点
    //计算刻度间隔及刻度起点
    switch (theMode) {
    case FixedValue:
        //该模式ValueSpace固定不变;
        valueSpace=fixedValueSpace;
        pxSpace=calcPxSpace(unit1PxToValue,valueSpace);
        pxBegin=calcPxBegin(unit1PxToValue,valueSpace,minValue,maxValue);
        break;
    case RefPixel:
        valueSpace=calcValueSpace(unit1PxToValue,refPixelSpace);
        pxSpace=calcPxSpace(unit1PxToValue,valueSpace);
        pxBegin=calcPxBegin(unit1PxToValue,valueSpace,minValue,maxValue);
        break;
    default:
        break;
    }
}

double XYAxis::calcPxSpace(double unitP2V, double valueSpace) const
{
    //这里与真0.0比较
    if(unitP2V<=0.0){
        qWarning()<<__FUNCTION__<<"unitP2V is too min"<<unitP2V;
        return 30.0;
    }
    return valueSpace/unitP2V;
}

double XYAxis::calcPxBegin(double unitP2V, double valueSpace, double valueMin, double valueMax) const
{
    if(unitP2V<=0.0||valueSpace<=0.0){
        qWarning()<<__FUNCTION__<<"unitP2V or valueSpace is too min"<<unitP2V<<valueSpace;
        return 0.0;
    }
    //min有正负，而unit和space只有正
    //如果最小值为正数or零
    //从最小值往上找第一个能被value_space整除的数
    //如果最小值为负数
    //从0往下找最后一个能被value_space整除的数
    //（如果min绝对值小于value_space则起点为0）
    //即起点值应该是value_space的整倍数
    const double begin_precision=std::pow(10,decimalPrecision);
    const double begin_cut=(decimalPrecision<=0)
            ?0
           :qRound(qAbs(valueMin)*begin_precision)%qRound(valueSpace*begin_precision)/begin_precision;
    //因为cut是value_space模出来的，且该分支min和value_space都为正，
    //所以起始值(value_space-cut)不会为负。
    //起点px就为起始值*单位值表示的像素；或者为起始值/单位像素表示的值
    //(注意：起始值是距离起点的间隔值)
    const double begin_val=qFuzzyIsNull(begin_cut)?0.0:(valueMin>=0.0)?(valueSpace-begin_cut):begin_cut;

    //注意横项和纵向不一样，Qt是屏幕坐标系，原点在左上角
    if(getAxisPosition()==AtTop||getAxisPosition()==AtBottom){
        //横向刻度值的是从左至右，和坐标x值增长方向一样
        return begin_val/unitP2V;
    }else{
        //竖向刻度值和坐标y值增长方向相反
        const double end_val=(valueMax-valueMin-begin_val)-valueSpace*(int)((valueMax-valueMin-begin_val)/valueSpace);
        return end_val/unitP2V;
    }
}

double XYAxis::calcValueSpace(double unitP2V, int pxRefSpace) const
{
    //尽量为整除
    const double space_ref=unitP2V*pxRefSpace;
    double space_temp=space_ref;
    if(space_ref>1)
        space_temp=calcValueSpaceHelper(space_ref,1);
    else
        space_temp=calcValueSpaceHelper(space_ref*std::pow(10,decimalPrecision),1)*std::pow(10,-decimalPrecision);
    //避免过大过小
    /*if(space_temp<=std::pow(10,-_decimalPrecision)){
        return std::pow(10,-_decimalPrecision);
    }else if(space_temp<space_ref*0.7){
        return space_temp*2;
    }else if(space_temp>space_ref*1.8){
        return space_temp/2;
    }*/
    return space_temp;
}

double XYAxis::calcValueSpaceHelper(double valueRefRange, int dividend) const
{
    //分段找合适的间隔，分割倍数dividend每次递归乘以10
    //考虑到当前应用场景，没有处理太大or太小的数
    //其实这个递归也不是很好，如果数值较大比较费时间，但是统计数值位数也需要去递归
    if(valueRefRange>8*dividend){
        //if(dividend>10000*100)return dividend;
        return calcValueSpaceHelper(valueRefRange,dividend*10);
    }else if(valueRefRange>4.5*dividend){
        return 5*dividend;
    }else if(valueRefRange>3*dividend){
        return 4*dividend;
    }else if(valueRefRange>1.5*dividend){
        return 2*dividend;
    }else{
        return dividend;
    }

    //递归思路
    /*if(temp_value>8*x){//x=1,>8--loop
        if(temp_value>8*x(10)){ //x=10,>80--loop
        }if(temp_value>4*x(10)){ //x=10,50
        }else if(temp_value>1.5*x(10)){ //x=10,20
        }else{ //x=10,10
        }
    }else if(temp_value>4*x){ //x=1,5
    }else if(temp_value>1.5*x){ //x=1,2
    }else{ //x=1,1
        //...
    }*/
}

int XYAxis::getTickPrecision() const
{
    return getTickPrecisionHelper(valueSpace,1,0);
}

int XYAxis::getTickPrecisionHelper(double valueSpace, double compare, int precision) const
{
    //如果大于compare，那么小数精度就是当前precision
    if(valueSpace>=compare){
        return precision;
    }
    return getTickPrecisionHelper(valueSpace,compare/10,precision+1);
}
