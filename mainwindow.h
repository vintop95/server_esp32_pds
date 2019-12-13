/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "chart.h"
#include "areaplot.h"
#include "settingsform.h"

#include <QMainWindow>

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

private:
    static MainWindow* instance;
    Ui::MainWindow* ui;
    Chart* chart;
    AreaPlot* areaPlot;
    AreaPlot* areaPlotMovements;
    QDateTimeEdit *startWindowDateTimeEdit = nullptr;
    QDateTimeEdit *endWindowDateTimeEdit = nullptr;
    QTableWidget *deviceFrequenciesTableWidget = nullptr;
    QTableWidget *correlatedDevicesTableWidget = nullptr;
    QTableWidget *hiddenMacCorrelationTableWidget = nullptr;

    explicit MainWindow(QWidget *parent = nullptr);
    void initChart();

    void initWindowDateTimeEdit();
    void setupPlot();
    void setupLogWindow();
    void initDeviceFrequenciesTableView();
    void loadDeviceFrequenciesInTableView(quint32 start_window, quint32 end_window);

signals:
    void logCurrDev();

private slots:
    void on_actionExit_triggered();
    void on_actionAbout_triggered();
    void on_actionPreferences_triggered();
    void on_logCurrentDevices_clicked();
    void on_actionDebug_triggered();

    void on_generatePackets_clicked();

    void on_startWindowDateTimeEdit_dateTimeChanged(const QDateTime &dateTime);

    void on_endWindowDateTimeEdit_dateTimeChanged(const QDateTime &dateTime);

    void on_ShowStatistics_clicked();

    void on_btnPlot_clicked();

    void on_btnFastBack_clicked();

    void on_btnBack_clicked();

    void on_btnForward_clicked();

    void on_btnFastForward_clicked();

public slots:
    // LOGGER
    void writeLogInUi(const QString &text, const QColor& = Qt::white);

public:
    ~MainWindow();
    static MainWindow* getInstance();
    Chart* getChart();
    AreaPlot* getAreaPlot();
    AreaPlot *getAreaPlotMovements();
    void plotHistory();
    void loadCorrelatedDevicesInTableView(QVector<QVector<Device> > correlatedDevices);
    void loadHiddenMacCorrelationInTableView(QVector<QVector<double> > hiddenMacCorrelation, QVector<Device> hiddenDevices);
};

#endif // MAINWINDOW_H
