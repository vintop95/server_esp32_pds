/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#ifndef CHART_H
#define CHART_H

#include <QtCharts>
using namespace QtCharts;

/**
 * It models the chart showing the devices
 */
class Chart: public QChart
{
    Q_OBJECT
public:
    Chart(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0);
    virtual ~Chart();

public slots:
    void updateChart(int countCurrDev);

private:
    QSplineSeries *m_series;
    QStringList m_titles;
    QValueAxis *m_valueAxisY;
    QDateTimeAxis *m_timeAxisX;
    qreal m_step;
    QDateTime m_x;
    int m_y;
};

#endif // CHART_H
