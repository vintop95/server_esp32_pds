#ifndef DEVICEFINDER_H
#define DEVICEFINDER_H

#include "main.h"
#include <QVector>
#include <QVector2D>

struct Record{
    QString sender_mac;
    quint32 timestamp;
    qint8 rssi;
    QString hashed_pkt;
    QString ssid;
};

class Device{
public:
};

class ESP32{
private:
    QString name;
    QVector2D pos;
public:
    ESP32(QString n, QVector2D p = QVector2D(0,0)):
    name(n), pos(p){ }
    void setPos(float xpos, float ypos){
        pos = QVector2D(xpos, ypos);
    }
};

class DeviceFinder : public QThread
{
private:
    int ESPNo;
    QVector<Record> records;
    QMap<QString, ESP32> esp;
public:
    DeviceFinder(int);
    //void run() override;
    void setESPPos(QString ESPName, float xpos, float ypos);
    void pushRecord(Record r);
};

#endif // DEVICEFINDER_H
