#include "areachart.h"

/**
 * @brief Constructor
 *
 * @param parent object
 */
AreaChart::AreaChart(QChart *parent) : QChart(parent)
{
    qDebug() << "#AreaChart";
    deviceSeries = new QScatterSeries();
    deviceSeries->setName("Device");
    deviceSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    deviceSeries->setMarkerSize(15.0);

    ESPSeries = new QScatterSeries();
    ESPSeries->setName("ESP32");
    ESPSeries->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    ESPSeries->setMarkerSize(20.0);

    for (ESP32 e: *(pSet->espList)){
        ESPSeries->append(e.getX(), e.getY());
    }

    addSeries(deviceSeries);
    addSeries(ESPSeries);

    setTitle("Devices in the area");

    createDefaultAxes();
    setDropShadowEnabled(false);

    legend()->setMarkerShape(QLegend::MarkerShapeFromSeries);

    qDebug() << "created.";
}

/**
 * @brief Add the device into the chart
 *
 * @param Device
 */
void AreaChart::appendDevice(Device d)
{
    deviceSeries->append(d.pos.x(), d.pos.y());
}
