#include "areachart.h"

bool compareHeight(const ESP32 &e1, const ESP32 &e2)
{
    return e1.getY() > e2.getY();
}

AreaChart::AreaChart(QChart *parent) : QChart(parent)
{
    //![1]
        QLineSeries *seriesHigh = new QLineSeries();
        QLineSeries *seriesLow = new QLineSeries();
    //![1]

    QList<ESP32> list;
    for (auto e: *(pSet->espList)){
        list.append(e);
    }
    std::sort(list.begin(), list.end(), compareHeight);

    int i=0;
    for(ESP32 e : list){
        i++;
        if(i <= ESP32_NO/2 ){
            *seriesLow << QPointF(e.getX(), e.getY());
        }else{
            *seriesHigh << QPointF(e.getX(), e.getY());
        }
    }

    //![3]
        QAreaSeries *series = new QAreaSeries(seriesHigh, seriesLow);
        series->setName("ESP32Area");
        QPen pen(0x059605);
        pen.setWidth(3);
        series->setPen(pen);

        QLinearGradient gradient(QPointF(0, 0), QPointF(0, 1));
        gradient.setColorAt(0.0, 0x3cc63c);
        gradient.setColorAt(1.0, 0x26f626);
        gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
        series->setBrush(gradient);
    //![3]

    //![4]
        addSeries(series);
        setTitle("Areachart of devices");
        createDefaultAxes();
        axisX()->setRange(0, 20);
        axisY()->setRange(0, 10);
    //![4]
}
