/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"

/**
 * @brief Constructor
 *
 * @param parent: defines the parent object
 * it's useful because the parent will have the task
 * to DELETE the object (making easy memory management)
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set the background of the log window black
    QPalette p = ui->txtLog->palette();
    p.setColor(QPalette::Base, QColor(20, 20, 20));
    ui->txtLog->setPalette(p);

    initChart();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief It writes the log message in the log window
 *
 * @param text to write
 * @param color of the text
 */
void MainWindow::writeLog(const QString &text, const QColor& color)
{
    ui->txtLog->setTextColor( color );
    ui->txtLog->append(text);
}

/**
 * @brief Returns the chart
 */
Chart* MainWindow::getChart()
{
    return chart;
}

/**
 * @brief Initializes the chart
 */
void MainWindow::initChart()
{
    chart = new Chart();
    chart->setTitle("DEVICES FOUND OVER TIME");

    ui->chartView->setChart(chart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);
}

/**
 * @brief Callback of pressing exit in the menu
 */
void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::quit();
}

/**
 * @brief Callback of pressing about in the menu
 */
void MainWindow::on_actionAbout_triggered()
{
    QString text;
    text.append("This program was brought to you by: \n\n");
    text.append("Gianluca D'Alleo\n");
    text.append("Salvatore Di Cara\n");
    text.append("Giorgio Pizzuto\n");
    text.append("Vincenzo Topazio");

    QMessageBox::about(this, "About ESP32 Server",text);
}
