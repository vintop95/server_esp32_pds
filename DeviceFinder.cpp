#include "DeviceFinder.h"

DeviceFinder::DeviceFinder(int n, int p): ESPNo(n), chartPeriod(p)
{
    for(int i=0; i<ESPNo; ++i){
        QString newName = "ESP" + QString::number(i);
        esp.insert(
           newName,
           ESP32(newName) );
    }
    initChart();
}

void DeviceFinder::setESPPos(QString devName, float xpos, float ypos)
{
    writeLog("Set " + devName + " pos: ("
             + QString::number(xpos) +  ","
             + QString::number(ypos) + ")");
    esp.find(devName).value().setPos(xpos, ypos);
}

void DeviceFinder::pushRecord(Record r)
{
    writeLog("Received pkt " + r.toString());

    // IPOTESI: un dispositivo non invia due volte
    // lo stesso pacchetto (secondo la definzione di operator==)


    //check if hash content already exists
    //controllerei prima il sender_mac per
    //vedere se il dispositivo è stato trovato almeno una volta
    if( records.count(r) < ESPNo-1 ){
        //we haven't reached yet all packets needed
        records.push_back(r);
    }else{
        //we have all packets needed to calculate the position
        records.push_back(r);
        QPointF p = calculatePosition(r);
        pushDevice(Device(r.sender_mac, p));
    }
}

void DeviceFinder::pushDevice(Device d)
{
    writeLog("Added/updated device " + d.sender_mac);

    // Insert functioning
    // If there is already an item with the key, that item's
    // value is replaced with value.
    devices.insert(d.sender_mac, d);
}

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

void DeviceFinder::initChart()
{
    //TODO: questa linea fa crashare
    //QObject::connect(&timer, &QTimer::timeout, pWin->getChart(), &Chart::updateChart);
    timer.setInterval(chartPeriod);
    timer.start();
}

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

// https://forums.estimote.com/t/determine-accurate-distance-of-signal/2858/2
// https://gist.github.com/eklimcz/446b56c0cb9cfe61d575
// https://stackoverflow.com/questions/20416218/understanding-ibeacon-distancing/20434019#20434019

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

QPointF DeviceFinder::calculatePosition(Record r)
{
    QPointF pos(0,0);

    pos.setX(-r.rssi);
    pos.setY(-r.rssi);
    //TODO: completare

    return pos;
}
