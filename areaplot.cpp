#include "areaplot.h"

AreaPlot::AreaPlot(QCustomPlot* cp, QObject *parent) :
    QObject(parent),
    customPlot(cp)
{
    // add the phase tracer (red circle) which sticks to the graph data:
    this->phaseTracer = new QCPItemTracer(this->customPlot);
    this->phaseTracer->setInterpolating(false);
    this->phaseTracer->setStyle(QCPItemTracer::tsCircle);
    this->phaseTracer->setPen(QPen(Qt::red));
    this->phaseTracer->setBrush(Qt::red);
    this->phaseTracer->setSize(10);

    connect(this->customPlot,
            SIGNAL(mouseMove(QMouseEvent*)),
            this,
            SLOT(showPointValue(QMouseEvent*)));
}

void AreaPlot::showPointValue( QMouseEvent* event )
{
    QCPGraph *graph = this->customPlot->graph(1);

    // Get selected graph (in my case selected means the plot is selected from the legend)
    for (int i=0; i<this->customPlot->graphCount(); ++i)
    {
        if( this->customPlot->legend->itemWithPlottable(this->customPlot->graph(i))->selected() )
        {
            graph = this->customPlot->graph(i);
            break;
        }
    }

    // Setup the item tracer
    this->phaseTracer->setGraph(graph);
    this->phaseTracer->setGraphKey(this->customPlot->xAxis->pixelToCoord(event->pos().x()));
    this->customPlot->replot();

    // **********Get the values from the item tracer's coords***********
    QPointF temp = this->phaseTracer->position->coords();

    int indexDevice = vecDevPos.indexOf(temp);
    QString strValue = "";
    if (indexDevice < vecDevValues.size() && indexDevice >= 0) {
        strValue = vecDevValues[indexDevice];
    }

    // Show a tooltip which tells the values
    QToolTip::showText(event->globalPos(),
                        tr("<h4>%L1</h4>"
                        "<table>"
                            "<tr>"
                                "<td><h5>X: %L2</h5></td>" "<td>  ,  </td>" "<td><h5>Y: %L3</h5></td>"
                            "</tr>"
                        "</table>")
    .arg( strValue )
    .arg( QString::number( temp.x(), 'f', 2 ) )
    .arg( QString::number( temp.y(), 'f', 2 ) ), customPlot, customPlot->rect());
}

//////////////////////////////////// Plot functions
///
void AreaPlot::addDevice(double x, double y, QString value)//aggiunge un punto ai QVector
{
    vecDevPos.append(QPointF(x,y));
    vecDevX.append(x);
    vecDevY.append(y);
    vecDevValues.append(value);
    replot();
}

void AreaPlot::removeDevice(double x, double y)
{
    for(int i=0; i<vecDevX.size(); i++){
        if( qFuzzyCompare(vecDevX[i], x) && qFuzzyCompare(vecDevY[i], y)){//se il punto è presente lo elimina
            vecDevPos.remove(i);
            vecDevX.remove(i);
            vecDevY.remove(i);
            vecDevValues.remove(i);
            break;
        }
    }
    replot();
}

void AreaPlot::addESP32(double x, double y)
{
    vecEspX.append(x);
    vecEspY.append(y);
    replot();
}

void AreaPlot::removeESP32(double x, double y)
{
    for(int i=0; i<vecEspX.size(); i++){
        if( qFuzzyCompare(vecEspX[i], x) && qFuzzyCompare(vecEspY[i], y)){//se il punto è presente lo elimina
            vecEspX.remove(i);
            vecEspY.remove(i);
            break;
        }
    }
    replot();
}

void AreaPlot::clearPlot()//svuota il grafico
{
    vecDevPos.clear();
    vecDevX.clear();
    vecDevY.clear();
    vecDevValues.clear();
    vecEspX.clear();
    vecEspY.clear();
    replot();
}

void AreaPlot::clearDevices()//svuota il grafico dei device
{
    vecDevPos.clear();
    vecDevX.clear();
    vecDevY.clear();
    vecDevValues.clear();
    replot();
}


void AreaPlot::replot()//aggiorna il grafico
{
    customPlot->graph(0)->setData(vecEspX, vecEspY);
    customPlot->graph(1)->setData(vecDevX, vecDevY);
    //variabili per ridimensionare la finestra correttamente
    double max_x_p = *std::max_element(vecDevX.begin(), vecDevX.end());
    double min_x_p = *std::min_element(vecDevX.begin(), vecDevX.end());
    double max_y_p = *std::max_element(vecDevY.begin(), vecDevY.end());
    double min_y_p  =  *std::min_element(vecDevY.begin(), vecDevY.end());
    double max_x_s = *std::max_element(vecEspX.begin(), vecEspX.end());
    double min_x_s = *std::min_element(vecEspX.begin(), vecEspX.end());
    double max_y_s = *std::max_element(vecEspY.begin(), vecEspY.end());
    double min_y_s  =  *std::min_element(vecEspY.begin(), vecEspY.end());
    double max_x = std::max(max_x_p, max_x_s);
    double min_x= std::min(min_x_p, min_x_s);
    double max_y = std::max(max_y_p, max_y_s);
    double min_y= std::min(min_y_p, min_y_s);
    //
    // customPlot->xAxis->setRange(min_x-1,max_x+1);
    // customPlot->yAxis->setRange(min_y -1,max_y+1);

    customPlot->xAxis->setRange(-2,5);
    customPlot->yAxis->setRange(-2,5);


    customPlot->replot();
    customPlot->update();
}
