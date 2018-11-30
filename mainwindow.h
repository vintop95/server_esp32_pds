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
    Ui::MainWindow *ui;
    Chart *chart;

    QVector<double> qv_x, qv_y, qvs_x, qvs_y;

    explicit MainWindow(QWidget *parent = nullptr);
    void initChart();

    void plot();

signals:
    void logCurrDev();

private slots:
    void on_actionExit_triggered();
    void on_actionAbout_triggered();
    void on_actionPreferences_triggered();
    void on_logCurrentDevices_clicked();
    void on_actionDebug_triggered();

public slots:
    // LOGGER
    void writeLogInUi(const QString &text, const QColor& = Qt::white);

public:
    ~MainWindow();
    static MainWindow* getInstance();
    Chart* getChart();

    // Plot management
    void addDevice(double x, double y);
    void removeDevice(double x, double y);
    void addESP32(double x, double y);
    void removeESP32(double x, double y);
    void clearPlot();
};

#endif // MAINWINDOW_H
