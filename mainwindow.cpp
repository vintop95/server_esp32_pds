/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "logger.h"


MainWindow* MainWindow::instance;

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

    // Setup the plot
    ui->plot->addGraph();
    ui->plot->graph(0)->setScatterStyle(QCPScatterStyle::ssCross);//stampa delle croci
    ui->plot->graph(0)->setPen(QPen(Qt::red));//penna rossa
    ui->plot->graph(0)->setLineStyle(QCPGraph::lsNone);//non connessi da nessuna linea
    ui->plot->addGraph();
    ui->plot->graph(1)->setScatterStyle(QCPScatterStyle::ssCircle);//stampa dei cerchietti
    ui->plot->graph(1)->setPen(QPen(Qt::blue));//penna blu
    ui->plot->graph(1)->setLineStyle(QCPGraph::lsNone);//non connessi da nessuna linea

    Settings* pSet = Settings::getInstance();
    for (ESP32 e: *(pSet->espList)){
        addBoard(e.getX(), e.getY());
    }

    initChart();

    writeLog("MAIN WINDOW CREATED");
}

MainWindow *MainWindow::getInstance()
{
    if (instance == nullptr){
        instance = new MainWindow();
    }
    return instance;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete chart;
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

//////////////////////////////////// Plot functions
///
void MainWindow::addPoint(double x, double y)//aggiunge un punto ai QVector
{
    qv_x.append(x);
    qv_y.append(y);
    plot();
}

void MainWindow::removePoint(double x, double y)
{
    for(int i=0; i<qv_x.size(); i++){
        if( qFuzzyCompare(qv_x[i], x) && qFuzzyCompare(qv_y[i], y)){//se il punto è presente lo elimina
            qv_x.remove(i);
            qv_y.remove(i);
            break;
        }
    }
    plot();
}

void MainWindow::addBoard(double x, double y)
{
    qvs_x.append(x);
    qvs_y.append(y);
    plot();
}

void MainWindow::removeBoard(double x, double y)
{
    for(int i=0; i<qvs_x.size(); i++){
        if( qFuzzyCompare(qvs_x[i], x) && qFuzzyCompare(qvs_y[i], y)){//se il punto è presente lo elimina
            qvs_x.remove(i);
            qvs_y.remove(i);
            break;
        }
    }
    plot();
}

void MainWindow::clearData()//svuota il grafico
{
    qv_x.clear();
    qv_y.clear();
    qvs_x.clear();
    qvs_y.clear();
    plot();
}

void MainWindow::plot()//aggiorna il grafico
{
    ui->plot->graph(0)->setData(qvs_x, qvs_y);
    ui->plot->graph(1)->setData(qv_x, qv_y);
    //variabili per ridimensionare la finestra correttamente
    double max_x_p = *std::max_element(qv_x.begin(), qv_x.end());
    double min_x_p = *std::min_element(qv_x.begin(), qv_x.end());
    double max_y_p = *std::max_element(qv_y.begin(), qv_y.end());
    double min_y_p  =  *std::min_element(qv_y.begin(), qv_y.end());
    double max_x_s = *std::max_element(qvs_x.begin(), qvs_x.end());
    double min_x_s = *std::min_element(qvs_x.begin(), qvs_x.end());
    double max_y_s = *std::max_element(qvs_y.begin(), qvs_y.end());
    double min_y_s  =  *std::min_element(qvs_y.begin(), qvs_y.end());
    double max_x = std::max(max_x_p, max_x_s);
    double min_x= std::min(min_x_p, min_x_s);
    double max_y = std::max(max_y_p, max_y_s);
    double min_y= std::min(min_y_p, min_y_s);
    //
    ui->plot->xAxis->setRange(min_x-1,max_x+1);
    ui->plot->yAxis->setRange(min_y -1,max_y+1);
    ui->plot->replot();
    ui->plot->update();
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

/**
 * @brief Open preferences form
 */
void MainWindow::on_actionPreferences_triggered()
{
    SettingsForm* settingsForm = new SettingsForm(SETTINGS_PATH, this);
    settingsForm->show();
}

/**
 * @brief Print the log of the current devices
 */
void MainWindow::on_logCurrentDevices_clicked()
{
    emit logCurrDev();
}

void MainWindow::on_actionDebug_triggered()
{
    QWidget *debugWindow = new QWidget();

    debugWindow->resize(320, 240);

    debugWindow->setWindowTitle(
        QApplication::translate("toplevel", "Debug Commands"));

    QVBoxLayout *vlay = new QVBoxLayout(debugWindow);

    QDoubleSpinBox *bx_x = new QDoubleSpinBox();
    vlay->addWidget(bx_x);
    QDoubleSpinBox *bx_y = new QDoubleSpinBox();
    vlay->addWidget(bx_y);

    QPushButton *addDevice = new QPushButton("Add device");
    vlay->addWidget(addDevice);
    QPushButton *removeDevice = new QPushButton("Remove device");
    vlay->addWidget(removeDevice);
    QPushButton *addEsp = new QPushButton("Add ESP");
    vlay->addWidget(addEsp);
    QPushButton *removeEsp = new QPushButton("Remove ESP");
    vlay->addWidget(removeEsp);
    QPushButton *clear = new QPushButton("Clear");
    vlay->addWidget(clear);
    debugWindow->setLayout(vlay);

    connect(addDevice, &QPushButton::clicked,
        [=](){
            double x,y;
            x = bx_x->value();
            y = bx_y->value();
            writeLog("Added device at (" +
                     QString::number(x) + ", " +
                     QString::number(y) + ")");
            addPoint(x,y);
        });

    connect(removeDevice, &QPushButton::clicked,
        [=](){
            double x,y;
            x = bx_x->value();
            y = bx_y->value();
            writeLog("Removed device at (" +
                 QString::number(x) + ", " +
                 QString::number(y) + ")");
            removePoint(x,y);
        });

    connect(addEsp, &QPushButton::clicked,
        [=](){
            double x,y;
            x = bx_x->value();
            y = bx_y->value();
            writeLog("Added ESP Board at (" +
                 QString::number(x) + ", " +
                 QString::number(y) + ")");
            addBoard(x,y);
        });
    connect(removeEsp, &QPushButton::clicked,
        [=](){
            double x,y;
            x = bx_x->value();
            y = bx_y->value();
            writeLog("Removed ESP Board at (" +
                 QString::number(x) + ", " +
                 QString::number(y) + ")");
            removeBoard(x,y);
        });
    connect(clear, &QPushButton::clicked,
        [=](){
            writeLog("Plot cleared.");
            clearData();
        });

    debugWindow->show();
}
