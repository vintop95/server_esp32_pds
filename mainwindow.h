#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts>
#include "Chart.h"

using namespace QtCharts;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void writeLog(const QString &text, const QColor& = Qt::white);
    Chart* getChart();
private slots:
    void on_actionExit_triggered();

private:
    void initChart();
    Ui::MainWindow *ui;
    Chart *chart;
};

#endif // MAINWINDOW_H
