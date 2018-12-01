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
    areaPlot = new AreaPlot(ui->plot);

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
        areaPlot->addESP32(e.getX(), e.getY());
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

AreaPlot *MainWindow::getAreaPlot()
{
    return areaPlot;
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
            areaPlot->addDevice(x,y);
        });

    connect(removeDeviceBtn, &QPushButton::clicked,
        [=](){
            double x,y;
            x = bx_x->value();
            y = bx_y->value();
            writeLogInUi("Removed device at (" +
                 QString::number(x) + ", " +
                 QString::number(y) + ")");
            areaPlot->removeDevice(x,y);
        });

    connect(addEsp32Btn, &QPushButton::clicked,
        [=](){
            double x,y;
            x = bx_x->value();
            y = bx_y->value();
            writeLogInUi("Added ESP Board at (" +
                 QString::number(x) + ", " +
                 QString::number(y) + ")");
            areaPlot->addESP32(x,y);
        });
    connect(removeEsp32Btn, &QPushButton::clicked,
        [=](){
            double x,y;
            x = bx_x->value();
            y = bx_y->value();
            writeLogInUi("Removed ESP Board at (" +
                 QString::number(x) + ", " +
                 QString::number(y) + ")");
            areaPlot->removeESP32(x,y);
        });
    connect(clear, &QPushButton::clicked,
        [=](){
            writeLogInUi("Plot cleared.");
            areaPlot->clearPlot();
        });

    debugWindow->show();
}
