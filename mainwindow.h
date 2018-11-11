/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts>
#include "Chart.h"
#include "settingsform.h"
#include "areachart.h"

using namespace QtCharts;

namespace Ui {
class MainWindow;
}

/**
 * This class models the main window
 * that can be modified in the mainwindow.ui file
 *
 * Singleton pattern is applied
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT //needed for QObject derived classes

public:
    static MainWindow* getInstance();
    ~MainWindow();
    void writeLog(const QString &text, const QColor& = Qt::white);
    Chart* getChart();
    AreaChart* getAreaChart();
    void initAreaChart();
private slots:
    void on_actionExit_triggered();
    void on_actionAbout_triggered();
    void on_actionPreferences_triggered();
    void on_logCurrentDevices_clicked();
signals:
    void logCurrDev();
private:
    static MainWindow* instance;
    Ui::MainWindow *ui;
    Chart *chart;
    AreaChart *areaChart;
    explicit MainWindow(QWidget *parent = nullptr);
    void initChart();
};

#endif // MAINWINDOW_H
