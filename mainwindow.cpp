/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "devicefinder.h"

MainWindow* MainWindow::instance;

// HOW TF TO ADJUST TAB LAYOUT? https://www.youtube.com/watch?v=8JYEdXDhrTY

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
    // init Ui
    ui->setupUi(this);

    initWindowDateTimeEdit();
    initDeviceFrequenciesTableView();

    setupLogWindow();

    setupPlot();

    initChart();
}

void MainWindow::setupLogWindow() {
    // Set the background of the log window black
    QPalette p = ui->txtLog->palette();
    p.setColor(QPalette::Base, QColor(20, 20, 20));
    ui->txtLog->setPalette(p);
}

void MainWindow::initWindowDateTimeEdit() {
    startWindowDateTimeEdit = ui->startWindowDateTimeEdit;
    startWindowDateTimeEdit->setDateTime(QDateTime::currentDateTime().addSecs(-3600));
    startWindowDateTimeEdit->setDisplayFormat("yyyy/MM/dd hh:mm");
    startWindowDateTimeEdit->setCalendarPopup(true);

    endWindowDateTimeEdit = ui->endWindowDateTimeEdit;
    endWindowDateTimeEdit->setDateTime(QDateTime::currentDateTime());
    endWindowDateTimeEdit->setDisplayFormat("yyyy/MM/dd hh:mm");
    endWindowDateTimeEdit->setCalendarPopup(true);

    startWindowDateTimeEdit->setMinimumDate(QDate(2018,03,01));
    endWindowDateTimeEdit->setMinimumDate(QDate(2018,03,01));

    startWindowDateTimeEdit->setMaximumDate(endWindowDateTimeEdit->date());
    endWindowDateTimeEdit->setMinimumDate(startWindowDateTimeEdit->date());
    //endWindowDateTimeEdit->setMaximumDate(...);
}

/*
sender_mac, COUNT(*) AS frequency, "
"MIN(timestamp) AS start_subwindow, MAX(timestamp) AS end_subwindow"
               */
void MainWindow::initDeviceFrequenciesTableView() {
    deviceFrequenciesTableWidget = ui->deviceFrequenciesTableWidget;
    //deviceFrequenciesTableWidget->horizontalHeader()->setStretchLastSection(true);
    deviceFrequenciesTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    deviceFrequenciesTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    deviceFrequenciesTableWidget->setRowCount(0);
    deviceFrequenciesTableWidget->setColumnCount(4);

    // Add headers to table widget
    QStringList headers;
    headers << "MAC" << "frequency" << "start_subwindow" << "end_subwindow";
    deviceFrequenciesTableWidget->setHorizontalHeaderLabels(headers);
}


void MainWindow::loadDeviceFrequenciesInTableView(quint32 start_window, quint32 end_window) {
    DbManager* db = DeviceFinder::getInstance()->getDb();
    QList<DeviceFrequencyInWindow> deviceFrequencies = db->computeDeviceFrequencies(start_window, end_window);

    deviceFrequenciesTableWidget->clearContents();
    deviceFrequenciesTableWidget->setRowCount(deviceFrequencies.size());

    int row = 0;
    for (auto deviceFreq: deviceFrequencies) {
        QTableWidgetItem* i0 = new QTableWidgetItem(deviceFreq.sender_mac);
        i0->setTextAlignment(Qt::AlignHCenter);
        QTableWidgetItem* i1 = new QTableWidgetItem(QString::number(deviceFreq.frequency));
        i1->setTextAlignment(Qt::AlignHCenter);

        QDateTime time;
        time.setTime_t(deviceFreq.start_subwindow);
        QTableWidgetItem* i2 = new QTableWidgetItem(time.toString(Qt::SystemLocaleLongDate));
        i2->setTextAlignment(Qt::AlignHCenter);
        time.setTime_t(deviceFreq.end_subwindow);
        QTableWidgetItem* i3 = new QTableWidgetItem(time.toString(Qt::SystemLocaleLongDate));
        i3->setTextAlignment(Qt::AlignHCenter);

        deviceFrequenciesTableWidget->setItem(row, 0, i0);
        deviceFrequenciesTableWidget->setItem(row, 1, i1);
        deviceFrequenciesTableWidget->setItem(row, 2, i2);
        deviceFrequenciesTableWidget->setItem(row, 3, i3);
        row++;
    }

}

void MainWindow::setupPlot() {
    // init AreaPlot
    areaPlot = new AreaPlot(ui->plot);

    // Setup the 1st plot
    ui->plot->addGraph();
    ui->plot->graph(0)->setScatterStyle(QCPScatterStyle::ssCross);//stampa delle croci
    ui->plot->graph(0)->setPen(QPen(Qt::red));//penna rossa
    ui->plot->graph(0)->setLineStyle(QCPGraph::lsNone);//non connessi da nessuna linea
    ui->plot->addGraph();
    ui->plot->graph(1)->setScatterStyle(QCPScatterStyle::ssCircle);//stampa dei cerchietti
    ui->plot->graph(1)->setPen(QPen(Qt::blue));//penna blu
    ui->plot->graph(1)->setLineStyle(QCPGraph::lsNone);//non connessi da nessuna linea

    // Setup the 2nd plot
    ui->plot_movements->addGraph();
    ui->plot_movements->graph(0)->setScatterStyle(QCPScatterStyle::ssCross);//stampa delle croci
    ui->plot_movements->graph(0)->setPen(QPen(Qt::red));//penna rossa
    ui->plot_movements->graph(0)->setLineStyle(QCPGraph::lsNone);//non connessi da nessuna linea
    ui->plot_movements->addGraph();
    ui->plot_movements->graph(1)->setScatterStyle(QCPScatterStyle::ssCircle);//stampa dei cerchietti
    ui->plot_movements->graph(1)->setPen(QPen(Qt::blue));//penna blu
    ui->plot_movements->graph(1)->setLineStyle(QCPGraph::lsNone);//non connessi da nessuna linea

    // Insert ESP32 in the 1st plot from settings
    for (ESP32 e: *(Settings::getInstance()->esp32s)){
        areaPlot->addESP32(e.getX(), e.getY());
    }

    AreaPlot* plotMovements = new AreaPlot(ui->plot_movements);

    // Insert ESP32 in the 2nd plot from settings
    for (ESP32 e: *(Settings::getInstance()->esp32s)){
        plotMovements->addESP32(e.getX(), e.getY());
    }
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

AreaPlot *MainWindow::getAreaPlotMovements()
{
    return (AreaPlot*) ui->plot_movements;
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

void MainWindow::on_generatePackets_clicked()
{
    DeviceFinder::getInstance()->generatePackets();
}

void MainWindow::on_startWindowDateTimeEdit_dateTimeChanged(const QDateTime &dateTime)
{
    // Check if dateTime is less than endWindow
    ui->endWindowDateTimeEdit->setMinimumDateTime(dateTime);
}

void MainWindow::on_endWindowDateTimeEdit_dateTimeChanged(const QDateTime &dateTime)
{
    // Check if dateTime is greater than startWindow
    ui->startWindowDateTimeEdit->setMaximumDateTime(dateTime);
}

void MainWindow::on_ShowStatistics_clicked()
{
    quint32 startDateTimestamp = startWindowDateTimeEdit->dateTime().toTime_t();
    quint32 endDateTimestamp = endWindowDateTimeEdit->dateTime().toTime_t();
    loadDeviceFrequenciesInTableView(startDateTimestamp, endDateTimestamp);
}
