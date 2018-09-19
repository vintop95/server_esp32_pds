/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#include "dbmanager.h"

/**
 * @brief Create and opens a connection to the database
 *
 * @param path of the sqlite file
 */
DbManager::DbManager(const QString& path)
{
   db = QSqlDatabase::addDatabase("QSQLITE");
   db.setDatabaseName(path);

   if (!db.open())
   {
      writeLog("Connection with database fail", QtCriticalMsg);
   }
   else
   {
      writeLog("Database: connection ok");
   }

   if ( !db.tables().contains( "packet" ) ) {
       createTables();
   }
   test();
}

/**
 * @brief Creates the needed tables
 */
bool DbManager::createTables()
{
    QSqlQuery query;
    bool res = query.exec("CREATE TABLE packet(id integer primary key, sender_mac text)");
    writeLog("table created: " + QString::number(res) );

    return res;
}

/**
 * @brief Test database functions
 */
void DbManager::test()
{
    QSqlQuery query;
    bool res;

    query.prepare("INSERT INTO packet(id, sender_mac) "
                  "VALUES (:id, :sender_mac)");
    query.bindValue(":id", 1);
    query.bindValue(":sender_mac", "AA");
    res = query.exec();
    writeLog("packet inserted: " + QString::number(res) );


    query.exec("SELECT sender_mac FROM packet");
    writeLog("select result: " + QString::number(res) );

    while (query.next()) {
        QString sender_mac = query.value(0).toString();
        writeLog("sender_mac: " + sender_mac);
    }
}
