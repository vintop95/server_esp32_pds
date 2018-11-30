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

private slots:
    void on_actionExit_triggered();
    void on_actionAbout_triggered();
    void on_actionPreferences_triggered();
    void on_logCurrentDevices_clicked();
    void on_actionDebug_triggered();

signals:
    void logCurrDev();

public:
    static MainWindow* getInstance();
    ~MainWindow();
    void writeLog(const QString &text, const QColor& = Qt::white);
    Chart* getChart();

    // Plot management
    void addPoint(double x, double y);
    void removePoint(double x, double y);
    void addBoard(double x, double y);
    void removeBoard(double x, double y);
    void clearData();
    void plot();

};

#endif // MAINWINDOW_H
