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
    bool saveCsv(Record& record, const QString& path = "output.txt");
    void setPath(QString p);
    bool addPacket(Record r);
    bool addPackets(QVector<Record> recordVector);
    bool calculateAvgRssi();
};


#endif // DBMANAGER_H
