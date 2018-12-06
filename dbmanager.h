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
    bool createTables();
    bool addPacket(Packet p); //INEFFICIENT
    void test();

public:
    //DEVICE_FINDER
    DbManager(const QString& path);
    void setPath(QString p);
    bool insertPackets(QVector<Packet> packets);
    bool calculateAvgRssi(); //to calculate position of devices
    void test_2();
    uint lastTimestamp;
};


#endif // DBMANAGER_H
