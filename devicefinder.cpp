/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#include "devicefinder.h"

#include <limits>

DeviceFinder* DeviceFinder::instance;

/**
 * @brief Constructor of DeviceFinder
 * Initializes the ESP32 devices calling them for each n
 * ESPn (ESP0, ESP1, ESP2...)
 */
DeviceFinder::DeviceFinder():
    db("server_esp32_pds.sqlite3")
{
    setInit(0);

    // in order to let the command of printing
    // from the gui
    setWindow(MainWindow::getInstance());

    connect(pWin, &MainWindow::logCurrDev,
            this, &DeviceFinder::logCurrentDevices);

}

DeviceFinder* DeviceFinder::getInstance(int espNo, QString dbPath)
{
    if (instance == nullptr){
        instance = new DeviceFinder();
    }
    instance->setInit(espNo, dbPath);
    return instance;
}

void DeviceFinder::setInit(int espNo, QString dbPath)
{
    if(espNo != 0){
        ESPNo = espNo;
    }
    if(dbPath != "server_esp32_pds.sqlite3"){
        db.setPath(dbPath);
    }


    for(int i=0; esp.size()<ESPNo; ++i){
        QString newName = "ESP" + QString::number(i);
        esp.insert(
           newName,
           ESP32(newName) );
    }
}

/**
 * @brief Initialize the chart in the window
 * Sets a timer that is called every chartPeriod
 * and calls updateChart()
 */
void DeviceFinder::initChart()
{
    connect(&timer, &QTimer::timeout,
    [&](){
        pWin->getChart()->updateChart(this->countCurrentDevices());
    });
    timer.setInterval(CHART_PERIOD);
    timer.start();
}

/**
 * @brief Sets the windows containing the chart and
 * initializes the chart
 */
void DeviceFinder::setWindow(MainWindow *w)
{
    pWin = w;
    initChart();
}

/**
 * @brief Set the position of the devName esp
 *
 * @param name of the esp32 device
 * @param x position
 * @param y position
 */
void DeviceFinder::setESPPos(QString devName, double xpos, double ypos)
{
    writeLog("#DeviceFinder");
    writeLog("Set " + devName + " pos: ("
             + QString::number(xpos) +  ","
             + QString::number(ypos) + ")");
    esp.find(devName).value().setPos(xpos, ypos);
}

/**
 * @brief Pushes the record received from ClientHandler in a list
 * and checks if the packets with the same HASH are received from
 * all the esp32 clients, if yes add the device detected to a list
 *
 * @param Record to push
 *
 * @todo IPOTESI: un dispositivo non invia due volte
 * lo stesso pacchetto (secondo la definzione di operator==)
 * correggere eventualmente
 */
void DeviceFinder::pushRecord(Record r)
{
    writeLog("#DeviceFinder");
    writeLog("Received pkt\n" + r.toString());
    db.saveCsv(r);

    // IPOTESI: un dispositivo non invia due volte
    // lo stesso pacchetto (secondo la definzione di operator==)

    // records.count(r) checks how many records are there in the list
    // that satisfies the operator== definition of Record

    if( records.count(r) < ESPNo-1 ){
        // we haven't reached yet all packets needed
        records.push_back(r);
    }else{
        // we have all packets needed to calculate the position
        records.push_back(r);
        QPointF p = calculatePosition(r);
        pushDevice(Device(r.sender_mac, p));
    }
}

/**
 * @brief Push a new device into the list
 *
 * @param Device to add
 *
 * @todo save in record the actual position device
 */
void DeviceFinder::pushDevice(Device d)
{
    writeLog("#DeviceFinder");
    writeLog("Added/updated device " + d.sender_mac);

    // TODO: save in record the actual position device

    // devices.insert
    // If there is already an item with the key, that
    // item's value is replaced with value.
    devices.insert(d.sender_mac, d);
    pWin->addPoint(d.pos.x(),d.pos.y());

    // TODO: remove device after a timeout?
}

/**
 * @brief a log of the current devices
 *
 * @todo: perfezionare
 */
void DeviceFinder::logCurrentDevices()
{
    writeLog("+++ DEVICES FOUND: +++", QtInfoMsg);
    if(devices.isEmpty()){
        writeLog("NONE", QtInfoMsg);
    }
    for(auto d : devices){
        writeLog(d.toString(), QtInfoMsg);
    }
    writeLog("+++++++++++++++++", QtInfoMsg);
}

/**
 * @brief Counts the number of current devices in the area
 *
 * @return number of current devices in the area
 *
 * @todo completare
 */
int DeviceFinder::countCurrentDevices()
{
    return devices.size();
}


// https://forums.estimote.com/t/determine-accurate-distance-of-signal/2858/2
// https://gist.github.com/eklimcz/446b56c0cb9cfe61d575
// https://stackoverflow.com/questions/20416218/understanding-ibeacon-distancing/20434019#20434019
/**
 * @brief It calculates the distance between the ESP32 client and the device
 * using the rssi value
 *
 * @param rssi
 *
 * @return distance
 *
 * @todo completare
 */
double DeviceFinder::calculateDistance(int rssi) {

    // TX-power-level == RSSI at 1m distance
    // esp txPower
    // https://www.esp32.com/viewtopic.php?t=5359

    if (rssi >= 0) {
        return -1.0;
    }

    double txCalibratedPower = -59; //hard coded power value. Usually ranges between -59 to -65
    double ratio_db = txCalibratedPower - rssi;
    double ratio_linear =  pow(10, ratio_db / 10);

    double r = sqrt(ratio_linear);
    return r;
}

/**
 * @brief Auxiliary struct, for sorting Records by their rssi
 */
struct sort_rssi_desc
{
    inline bool operator() (const Record& r1, const Record& r2)
    {
        //writeLog(QString::number(r1.rssi) + ">" + QString::number(r2.rssi) + "?");
        return ( r1.rssi > r2.rssi );
    }
};

/**
 * @brief It calculates the position of the device identified by the
 * sender_mac in the Record
 *
 * https://trello.com/c/X2EOGoxG
 * HOW IT SHOULD WORK WITH SQL:
 * SQL Query returns (sender_mac, espName, avgRssi)
 * for all sender_mac
 *  we take the records that has the same sender_mac (and hashed_pkt)
 *  and we calculate the position with avgRssi
 * we push all the devices with their position in the 'device' table
 * (or in a QVector<Device>)
 *
 *  last_ts = the timestamp of the last time we calculated the positions
 *  of devices
 *
 * @param Record containing the device
 * it's the last received for calculation purposes from the last board
 *
 * @return Position in space of the device detected
 *
 * @todo completare
 */
QPointF DeviceFinder::calculatePosition(Record lastRecord)
{
    //TODO: completare
    writeLog("#DeviceFinder");

    QVector<Record> recordsOrdered;
    for(Record genericRecord : records){
        auto it = esp.find(genericRecord.espName);
        bool exists = (it != esp.end());
        if(exists && genericRecord == lastRecord){
            recordsOrdered.push_back(genericRecord);
        }
    }

    // take the 3 records with the most powerful rssi
    std::sort(recordsOrdered.begin(), recordsOrdered.end(), sort_rssi_desc());

    QList<ESP32> espChosen;
    writeLog("PRINTING RECORDS ORDERED FOR RSSI");
    for(Record recordOrdered : recordsOrdered){
        writeLog(recordOrdered.toString());

        ESP32 espToPush("");
        auto it = esp.find(recordOrdered.espName);
        bool exists = (it != esp.end());
        if(exists){
            espToPush = esp.value(recordOrdered.espName, ESP32(""));
            espChosen.push_back(espToPush);
        }
    }


//    // DEBUG: test di trilateration
//    QPointF pos = trilateration(QPointF(0,0), QPointF(2,0), QPointF(0,2),
//            1.0,
//            sqrt(5.0),
//            1.0);

    // check if espChosen contains all the esp needed
    QPointF pos;

    if(espChosen.size() <= 1){
        pos.setX(std::numeric_limits<double>::quiet_NaN());
        pos.setY(std::numeric_limits<double>::quiet_NaN());
    }else if(espChosen.size() == 2){
        // TODO: how to return two points?
        std::pair<QPointF,QPointF> pair = bilateration(
                espChosen[0].getPos(), espChosen[1].getPos(),
                calculateDistance(recordsOrdered[0].rssi),
                calculateDistance(recordsOrdered[1].rssi));
        pos = pair.first;
    }else{
        pos = trilateration(espChosen[0].getPos(), espChosen[1].getPos(), espChosen[2].getPos(),
                        calculateDistance(recordsOrdered[0].rssi),
                        calculateDistance(recordsOrdered[1].rssi),
                        calculateDistance(recordsOrdered[2].rssi));
    }

    // delete the existing record used for find the position
    records.removeAll(lastRecord);

    last_ts = QDateTime::currentDateTime().toTime_t();

    return pos;
}


double norm (QPointF p) // get the norm of a vector
{
    return pow( pow(p.x(),2) + pow(p.y(),2), .5);
}

void printPoint(QPointF p, QString name)
{
    writeLog(name + ":(" + QString::number(p.x(), 'f', 2) + "," + QString::number(p.y(), 'f', 2) + ")");

}

void printValue(double v, QString name)
{
    writeLog(name + ":" + QString::number(v, 'f', 2));
}

QPointF DeviceFinder::trilateration(QPointF p1, QPointF p2, QPointF p3, double r1, double r2, double r3)
{
    printPoint(p1, "p1");
    printPoint(p2, "p2");
    printPoint(p3, "p3");
    printValue(r1, "r1");
    printValue(r2, "r2");
    printValue(r3, "r3");

    QPointF resultPose;
    //unit vector in a direction from point1 to point 2
    double p2p1Distance = pow(pow(p2.x()-p1.x(),2) + pow(p2.y()-   p1.y(),2),0.5);
//    printValue(p2p1Distance, "p2p1Distance");
    // Ex ed ey sono i versori della direzione, aux e aux2 sono solo delle variabili ausiliarie
    QPointF ex = {(p2.x()-p1.x())/p2p1Distance, (p2.y()-p1.y())/p2p1Distance};
//    printPoint(ex, "ex");
    QPointF aux = {p3.x()-p1.x(),p3.y()-p1.y()};
//    printPoint(aux, "aux");
    //signed magnitude of the x component
    double i = ex.x() * aux.x() + ex.y() * aux.y();
//    printValue(i, "i");
    //the unit vector in the y direction.
    QPointF aux2 = { p3.x()-p1.x()-i*ex.x(), p3.y()-p1.y()-i*ex.y()};
//    printPoint(aux2, "aux2");
    QPointF ey = { aux2.x() / norm(aux2), aux2.y() / norm(aux2) };
//    printPoint(ey, "ey");
    //the signed magnitude of the y component
    double j = ey.x() * aux.x() + ey.y() * aux.y();
//    printValue(j, "j");
    //coordinates
    double x = (pow(r1,2) - pow(r2,2) + pow(p2p1Distance,2))/ (2 * p2p1Distance);

//    printValue(x, "x");
    double y = (pow(r1,2) - pow(r3,2) + pow(i,2) + pow(j,2))/(2*j) - i*x/j;
//    printValue(y, "y");
    //result coordinates
    double finalX = p1.x()+ x*ex.x() + y*ey.x();
    double finalY = p1.y()+ x*ex.y() + y*ey.y();
    finalX = finalX<0?0:finalX;
    finalY = finalY<0?0:finalY;
    finalX = finalX>10?10:finalX;
    finalY = finalY>10?10:finalY;
    resultPose.setX(finalX);
    resultPose.setY(finalY);
//    printPoint(resultPose, "resultPose");
    return resultPose;

}

double distance(QPointF point1, QPointF point2){
    return pow(pow(point2.x()-point1.x(), 2) + pow(point2.y()-point1.y(), 2), 0.5);
}
QPointF normal(QPointF p1) {
    double length = sqrt(p1.x()*p1.x() + p1.y()*p1.y());
    return QPointF(p1.x()/length, p1.y()/length);
}
QPointF scale(QPointF p, double s) {
    return QPointF(p.x()*s, p.y()*s);
}
QPointF sub(QPointF p1, QPointF p2) {
    return QPointF(p1.x() - p2.x(), p1.y() - p2.y());
}
QPointF add(QPointF p1, QPointF p2) {
    return QPointF(p1.x() + p2.x(), p1.y() + p2.y());
}

std::pair<QPointF, QPointF> DeviceFinder::bilateration(QPointF p1, QPointF p2, double r1, double r2){
    QPointF P0(p1.x(), p1.y());
    QPointF P1(p2.x(), p2.y());

    double d, a, h;
    d = distance(P0, P1);
    a = (r1*r1 - r2*r2 + d*d)/(2*d);
    h = sqrt(r1*r1 - a*a);
    QPointF P2 = add( scale( sub(P1, P0), a/d), P0);

    double x3, y3, x4, y4;
    x3 = P2.x() + h*(P1.y() - P0.y())/d;
    y3 = P2.y() - h*(P1.x() - P0.x())/d;
    x4 = P2.x() - h*(P1.y() - P0.y())/d;
    y4 = P2.y() + h*(P1.x() - P0.x())/d;

    return std::pair<QPointF, QPointF>(QPointF(x3, y3), QPointF(x4, y4));
}


/**
 * @brief A test function
 */
void DeviceFinder::test()
{
    Record r;

    for(int i=0; i<5; ++i){
        if(i==0){
            r.rssi = -50;
        }else if(i>=1){
            r.rssi = -70;
        }
        r.espName = "ESP" + QString::number(i);
        r.sender_mac = "TEST";
        r.timestamp = 10;
        r.hashed_pkt = "123456";
        pushRecord(r);
        logCurrentDevices();
    }
}
