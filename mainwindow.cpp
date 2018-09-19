#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QPalette p = ui->txtLog->palette();
    p.setColor(QPalette::Base, QColor(20, 20, 20));
    ui->txtLog->setPalette(p);

    initChart();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::writeLog(const QString &text, const QColor& color)
{
    ui->txtLog->setTextColor( color );
    ui->txtLog->append(text);
}

Chart* MainWindow::getChart()
{
    return chart;
}

void MainWindow::initChart()
{
    chart = new Chart();
    chart->legend()->hide();
    chart->setTitle("Devices found during time");
    chart->setAnimationOptions(QChart::AllAnimations);

    ui->chartView->setChart(chart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::quit();
}
