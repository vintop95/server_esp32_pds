#ifndef AREAPLOT_H
#define AREAPLOT_H

#include "qcustomplot.h"

#include <QObject>

class AreaPlot : public QObject
{
    Q_OBJECT

private:
    QCustomPlot* customPlot;
    QVector<double> vecDevX, vecDevY, vecEspX, vecEspY;

signals:

public slots:
    // Plot management
    void replot();
    void addDevice(double x, double y);
    void removeDevice(double x, double y);
    void addESP32(double x, double y);
    void removeESP32(double x, double y);
    void clearPlot();
public:
    explicit AreaPlot(QCustomPlot* customPlot, QObject *parent = nullptr);

};

#endif // AREAPLOT_H
