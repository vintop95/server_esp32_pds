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
    void test();

public:
    DbManager(const QString& path);
    bool saveCsv(Packet& packet, const QString& path = "output.txt");
    void setPath(QString p);
    bool addPacket(Packet r);
    bool addPackets(QVector<Packet> packets);
    bool calculateAvgRssi();
};


#endif // DBMANAGER_H
