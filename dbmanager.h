/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#ifndef DBMANAGER_H
#define DBMANAGER_H

#include "main.h"

#include <QObject>
#include <QtSql>

/**
 * Database manager
 */
class DbManager
{
private:
    QSqlDatabase db;
    void createTablesIfNotExist();
    void checkConsistencyAndCreateTable(QString TABLE_NAME, int nOfCols, QString strQueryCreate );
    bool addDevice(Device d);
    void test();

public:
    //DEVICE_FINDER
    DbManager(const QString& path);
    void setPath(QString p);
    bool insertPackets(QVector<Packet> packets);
    bool insertDevices(QList<Device> devices);
    avgRssiMap_t calculateAvgRssi(int espNumber, unsigned int lastTimestamp); //to calculate position of devices
    void test_2();
    //uint lastTimestamp;
};


#endif // DBMANAGER_H
