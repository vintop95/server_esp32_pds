/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#include "DeviceFinder.h"

/**
 * @brief Constructor of DeviceFinder
 * Initializes the ESP32 devices calling them for each n
 * ESPn (ESP0, ESP1, ESP2...)
 */
DeviceFinder::DeviceFinder(int n, int p, QString dbPath):
    ESPNo(n), chartPeriod(p), db(dbPath)
{
    for(int i=0; i<ESPNo; ++i){
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
        win->getChart()->updateChart(this->countCurrentDevices());
    });
    timer.setInterval(chartPeriod);
    timer.start();
}

/**
 * @brief Sets the windows containing the chart and
 * initializes the chart
 */
void DeviceFinder::setWindow(MainWindow *w)
{
    win = w;
    initChart();
}

/**
 * @brief Set the position of the devName esp
 *
 * @param name of the esp32 device
 * @param x position
 * @param y position
 */
void DeviceFinder::setESPPos(QString devName, float xpos, float ypos)
{
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
    writeLog("Received pkt\n" + r.toString());

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
 */
void DeviceFinder::pushDevice(Device d)
{
    writeLog("Added/updated device " + d.sender_mac);

    // devices.insert
    // If there is already an item with the key, that
    // item's value is replaced with value.
    devices.insert(d.sender_mac, d);
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
    // TODO: completare
    return 0;
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
double calculateDistance(int rssi) {

    // TX-power-level == RSSI at 1m distance
    // esp txPower
    // https://www.esp32.com/viewtopic.php?t=5359
    int txPower = -59; //hard coded power value. Usually ranges between -59 to -65

    if (rssi == 0) {
        return -1.0;
    }

    double ratio = rssi*1.0/txPower;
    double distance;
    if (ratio < 1.0) {
        return qPow(ratio,10);
    }
    else {
        distance =  (0.89976)* qPow(ratio,7.7095) + 0.111;
        return distance;
    }

    // oppure questa formula
    // distance in meters = pow(10, (RssiAtOneMeter - ReceivedRSSI) / 20)
    distance = qPow(10, (txPower - rssi) / 20);
}

/**
 * @brief It calculates the position of the device identified by the sender_mac
 * in the Record
 *
 * @param Record containing the device
 *
 * @return Position in space of the device detected
 *
 * @todo completare
 */
QPointF DeviceFinder::calculatePosition(Record r)
{
    //TODO: completare
    QPointF pos(0,0);

    pos.setX(-r.rssi);
    pos.setY(-r.rssi);

    return pos;
}

/**
 * @brief A test function
 */
void DeviceFinder::test()
{
    Record r;

    r.sender_mac = "AA";
    r.timestamp = 10;
    r.rssi = -35;
    r.hashed_pkt = "12345";
    r.espName = "ESP0";
    pushRecord(r);
    logCurrentDevices();

    r.sender_mac = "AA";
    r.timestamp = 10;
    r.rssi = -55;
    r.hashed_pkt = "12345";
    r.espName = "ESP1";
    pushRecord(r);
    logCurrentDevices();

    r.sender_mac = "AA";
    r.timestamp = 10;
    r.rssi = -55;
    r.hashed_pkt = "12345";
    r.espName = "ESP2";
    pushRecord(r);
    logCurrentDevices();

    r.sender_mac = "AA";
    r.timestamp = 10;
    r.rssi = -65;
    r.hashed_pkt = "12345";
    r.espName = "ESP3";
    pushRecord(r);
    logCurrentDevices();

    for(int i=0; i<5; ++i){
        r.sender_mac = "AA";
        r.timestamp = 10;
        r.rssi = -55;
        r.hashed_pkt = "123456";
        pushRecord(r);
        logCurrentDevices();
    }
}
