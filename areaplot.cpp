#include "areaplot.h"

AreaPlot::AreaPlot(QCustomPlot* cp, QObject *parent) :
    QObject(parent),
    customPlot(cp)
{

}


//////////////////////////////////// Plot functions
///
void AreaPlot::addDevice(double x, double y)//aggiunge un punto ai QVector
{
    vecDevX.append(x);
    vecDevY.append(y);
    replot();
}

void AreaPlot::removeDevice(double x, double y)
{
    for(int i=0; i<vecDevX.size(); i++){
        if( qFuzzyCompare(vecDevX[i], x) && qFuzzyCompare(vecDevY[i], y)){//se il punto è presente lo elimina
            vecDevX.remove(i);
            vecDevY.remove(i);
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
    vecDevX.clear();
    vecDevY.clear();
    vecEspX.clear();
    vecEspY.clear();
    replot();
}

void AreaPlot::clearDevices()//svuota il grafico dei device
{
    vecDevX.clear();
    vecDevY.clear();
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

    customPlot->xAxis->setRange(-1,3);
    customPlot->yAxis->setRange(-1,3);


    customPlot->replot();
    customPlot->update();
}
