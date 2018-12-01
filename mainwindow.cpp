/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"


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
    for (ESP32 e: *(pSet->esp32s)){
        addESP32(e.getX(), e.getY());
    }

    initChart();
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
void MainWindow::writeLogInUi(const QString &text, const QColor& color)
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
void MainWindow::addDevice(double x, double y)//aggiunge un punto ai QVector
{
    vecDevX.append(x);
    vecDevY.append(y);
    plot();
}

void MainWindow::removeDevice(double x, double y)
{
    for(int i=0; i<vecDevX.size(); i++){
        if( qFuzzyCompare(vecDevX[i], x) && qFuzzyCompare(vecDevY[i], y)){//se il punto è presente lo elimina
            vecDevX.remove(i);
            vecDevY.remove(i);
            break;
        }
    }
    plot();
}

void MainWindow::addESP32(double x, double y)
{
    vecEspX.append(x);
    vecEspY.append(y);
    plot();
}

void MainWindow::removeESP32(double x, double y)
{
    for(int i=0; i<vecEspX.size(); i++){
        if( qFuzzyCompare(vecEspX[i], x) && qFuzzyCompare(vecEspY[i], y)){//se il punto è presente lo elimina
            vecEspX.remove(i);
            vecEspY.remove(i);
            break;
        }
    }
    plot();
}

void MainWindow::clearPlot()//svuota il grafico
{
    vecDevX.clear();
    vecDevY.clear();
    vecEspX.clear();
    vecEspY.clear();
    plot();
}

void MainWindow::plot()//aggiorna il grafico
{
    ui->plot->graph(0)->setData(vecEspX, vecEspY);
    ui->plot->graph(1)->setData(vecDevX, vecDevY);
    //variabili per ridimensionare la finestra correttamente
    double max_x_p = *std::max_element(vecDevX.begin(), vecDevX.end());
    double min_x_p = *std::min_element(vecDevX.begin(), vecDevX.end());
    double max_y_p = *std::max_element(vecDevY.begin(), vecDevY.end());
    double min_y_p  =  *std::min_element(vecDevY.begin(), vecDevY.end());
    double max_x_s = *std::max_element(vecEspX.begin(), vecEspX.end());
    double min_x_s = *std::min_element(vecEspX.begin(), vecEspX.end());
    double max_y_s = *std::max_element(vecEspY.begin(), vecEspY.end());
    double min_y_s  =  *std::min_element(vecEspY.begin(), vecEspY.end());
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

    QPushButton *addDeviceBtn = new QPushButton("Add device");
    vlay->addWidget(addDeviceBtn);
    QPushButton *removeDeviceBtn = new QPushButton("Remove device");
    vlay->addWidget(removeDeviceBtn);
    QPushButton *addEsp32Btn = new QPushButton("Add ESP");
    vlay->addWidget(addEsp32Btn);
    QPushButton *removeEsp32Btn = new QPushButton("Remove ESP");
    vlay->addWidget(removeEsp32Btn);
    QPushButton *clear = new QPushButton("Clear");
    vlay->addWidget(clear);
    debugWindow->setLayout(vlay);

    connect(addDeviceBtn, &QPushButton::clicked,
        [=](){
            double x,y;
            x = bx_x->value();
            y = bx_y->value();
            writeLogInUi("Added device at (" +
                     QString::number(x) + ", " +
                     QString::number(y) + ")");
            addDevice(x,y);
        });

    connect(removeDeviceBtn, &QPushButton::clicked,
        [=](){
            double x,y;
            x = bx_x->value();
            y = bx_y->value();
            writeLogInUi("Removed device at (" +
                 QString::number(x) + ", " +
                 QString::number(y) + ")");
            removeDevice(x,y);
        });

    connect(addEsp32Btn, &QPushButton::clicked,
        [=](){
            double x,y;
            x = bx_x->value();
            y = bx_y->value();
            writeLogInUi("Added ESP Board at (" +
                 QString::number(x) + ", " +
                 QString::number(y) + ")");
            addESP32(x,y);
        });
    connect(removeEsp32Btn, &QPushButton::clicked,
        [=](){
            double x,y;
            x = bx_x->value();
            y = bx_y->value();
            writeLogInUi("Removed ESP Board at (" +
                 QString::number(x) + ", " +
                 QString::number(y) + ")");
            removeESP32(x,y);
        });
    connect(clear, &QPushButton::clicked,
        [=](){
            writeLogInUi("Plot cleared.");
            clearPlot();
        });

    debugWindow->show();
}
