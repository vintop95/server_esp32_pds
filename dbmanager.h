/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QtSql>
#include "main.h"

/**
 * Database manager
 */
class DbManager
{
public:
    DbManager(const QString& path);
private:
    QSqlDatabase db;
    bool createTables();
    void test();
};


#endif // DBMANAGER_H
