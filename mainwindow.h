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

    explicit MainWindow(QWidget *parent = nullptr);
    void initChart();

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
    AreaPlot* getAreaPlot();
};

#endif // MAINWINDOW_H
