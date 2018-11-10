#include "areachart.h"

bool compareHeight(const ESP32 &e1, const ESP32 &e2)
{
    return e1.getY() > e2.getY();
}

AreaChart::AreaChart(QChart *parent) : QChart(parent)
{

    QScatterSeries *deviceSeries = new QScatterSeries();
    deviceSeries->setName("Device");
    deviceSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    deviceSeries->setMarkerSize(15.0);

    QScatterSeries *ESPSeries = new QScatterSeries();
    ESPSeries->setName("ESP32");
    ESPSeries->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    ESPSeries->setMarkerSize(20.0);


    for (ESP32 e: *(pSet->espList)){
        ESPSeries->append(e.getX(), e.getY());
    }



    *deviceSeries << QPointF(1, 1) << QPointF(3, 3) << QPointF(7, 6) << QPointF(8, 3) << QPointF(10, 2);


    addSeries(deviceSeries);
    addSeries(ESPSeries);

    setTitle("Devices in the area");
    createDefaultAxes();
    setDropShadowEnabled(false);

    legend()->setMarkerShape(QLegend::MarkerShapeFromSeries);
}
