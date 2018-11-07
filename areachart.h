#ifndef AREACHART_H
#define AREACHART_H

//#include "logger.h"
#include <QObject>

#include <QtWidgets>
#include <QtCharts>

#include "settings.h"


class AreaChart : public QChart
{
    Q_OBJECT
public:
    explicit AreaChart(QChart *parent = nullptr);
    void appendDevice(Device d);
signals:

public slots:

private:
    QScatterSeries *deviceSeries;
    QScatterSeries *ESPSeries;
};

#endif // AREACHART_H
