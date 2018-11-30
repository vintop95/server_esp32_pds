/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#include "chart.h"
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QValueAxis>
#include <QtCore/QRandomGenerator>
#include <QtCore/QDebug>
#include <QDate>

/**
 * @brief Constructor
 *
 * @param parent object
 */
Chart::Chart(QGraphicsItem *parent, Qt::WindowFlags wFlags):
    QChart(QChart::ChartTypeCartesian, parent, wFlags),
    m_series(nullptr),
    m_valueAxisY(new QValueAxis(this)),
    m_timeAxisX(new QDateTimeAxis(this)),
    m_step(0),
    m_x(START_TIME),
    m_y(0)
{
    qDebug() << "#Chart";
    // The series of values
    m_series = new QSplineSeries(this);

    // Color the line in red
    QPen pen(Qt::red);
    pen.setWidth(3);
    m_series->setPen(pen);

    // Initial value
    m_series->append(m_x.toMSecsSinceEpoch(), m_y);
    m_timeAxisX->setFormat("hh:mm:ss");
    addSeries(m_series);
    createDefaultAxes();
    setAxisX(m_timeAxisX, m_series);
    setAxisY(m_valueAxisY, m_series);

    // Set initial view of the graph
    m_valueAxisY->setTickCount(6);
    m_timeAxisX->setTickCount(10);
    axisX()->setRange(m_x.addSecs(-CHART_PERIOD_MS/1000*10), m_x.addSecs(0));
    axisY()->setRange(0, 10);


    legend()->hide();
    setAnimationOptions(QChart::SeriesAnimations);
    qDebug() << "created.";
}

Chart::~Chart()
{

}

/**
 * @brief Callback to be called every chartPeriod
 * to update the chart
 *
 * @param number of current devices
 *
 * @todo completare
 */
void Chart::updateChart(int countCurrDev)
{
    qDebug() << "#Chart";
    qDebug() << "update number of devices.";
    // random generator
    //QRandomGenerator::global()->bounded(5) - 2.5

    qreal x = plotArea().width() / m_timeAxisX->tickCount();
    qreal y = (m_timeAxisX->max().toMSecsSinceEpoch() - m_timeAxisX->min().toMSecsSinceEpoch()) / m_timeAxisX->tickCount() / 1000;
    m_x = m_x.addSecs(static_cast<qint64>(y));
    m_y = countCurrDev;
    //qDebug() << m_x.toTime_t()*1000;
    m_series->append(m_x.toMSecsSinceEpoch(), m_y);
    scroll(x, 0);
}
