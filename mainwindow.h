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

using namespace QtCharts;

namespace Ui {
class MainWindow;
}

/**
 * This class models the main window
 * that can be modified in the mainwindow.ui file
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT //needed for QObject derived classes

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void writeLog(const QString &text, const QColor& = Qt::white);
    Chart* getChart();
private slots:
    void on_actionExit_triggered();
    void on_actionAbout_triggered();
    void on_actionPreferences_triggered();

private:
    void initChart();
    Ui::MainWindow *ui;
    Chart *chart;
};

/**
 * Nedded to have a singleton global class
 */
extern MainWindow* pWin;

#endif // MAINWINDOW_H
