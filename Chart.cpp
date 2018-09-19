/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#include "Chart.h"
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QValueAxis>
#include <QtCore/QRandomGenerator>
#include <QtCore/QDebug>

/**
 * @brief Constructor
 *
 * @param parent object
 */
Chart::Chart(QGraphicsItem *parent, Qt::WindowFlags wFlags):
    QChart(QChart::ChartTypeCartesian, parent, wFlags),
    m_series(0),
    m_axis(new QValueAxis),
    m_step(0),
    m_x(0),
    m_y(0)
{
    // The series of values
    m_series = new QSplineSeries(this);

    // Color the line in red
    QPen pen(Qt::red);
    pen.setWidth(3);
    m_series->setPen(pen);

    // Initial value
    m_series->append(m_x, m_y);

    addSeries(m_series);
    createDefaultAxes();
    setAxisX(m_axis, m_series);

    // Set initial view of the graph
    m_axis->setTickCount(10);
    axisX()->setRange(-4, 5);
    axisY()->setRange(-5, 10);

    legend()->hide();
    setAnimationOptions(QChart::AllAnimations);
}

Chart::~Chart()
{

}

/**
 * @brief Callback to be called every chartPeriod
 * to update the chart
 *
 * @param numbero of current devices
 *
 * @todo completare
 */
void Chart::updateChart(int countCurrDev)
{
    qreal x = plotArea().width() / m_axis->tickCount();
    qreal y = (m_axis->max() - m_axis->min()) / m_axis->tickCount();
    m_x += y;
    m_y = QRandomGenerator::global()->bounded(5) - 2.5;
    m_series->append(m_x, m_y);
    scroll(x, 0);
}
