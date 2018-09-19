#ifndef CHART_H
#define CHART_H

#include <QtCharts>
//#include "DeviceFinder.h"

using namespace QtCharts;

class Chart: public QChart
{
    Q_OBJECT
public:
    Chart(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0);
    virtual ~Chart();

public slots:
    void updateChart();

private:
    QSplineSeries *m_series;
    QStringList m_titles;
    QValueAxis *m_axis;
    qreal m_step;
    qreal m_x;
    qreal m_y;
};

#endif // CHART_H
