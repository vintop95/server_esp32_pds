#include "DeviceFinder.h"

DeviceFinder::DeviceFinder(int n): ESPNo(n)
{
    for(int i=0; i<ESPNo; ++i){
        QString newName = "ESP" + QString::number(i);
        esp.insert(
           newName,
           ESP32(newName) );
    }
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

QPointF DeviceFinder::calculatePosition(Record r)
{
    QPointF pos(0,0);

    pos.setX(-r.rssi);
    pos.setY(-r.rssi);
    //TODO: completare

    return pos;
}
