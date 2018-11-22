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
    // TODO: calculate the nOfCols from the CREATE query
    int nOfCols = 7;

   db = QSqlDatabase::addDatabase("QSQLITE");
   db.setDatabaseName(path);

   writeLog("#DbManager");
   if (!db.open())
   {
      writeLog("Connection with database fail", QtCriticalMsg);
   }
   else
   {
      writeLog("connection ok");
   }

   if ( !db.tables().contains( "packet" ) ) {
       createTables();
   }else{
       // it must contain all the attributes
       // defined: the number of columns of the two
       // version of packet must be equal
       QSqlRecord record = db.driver()->record("packet");
       if (record.count() != nOfCols){
            createTables();
       };
   }
   test();
}

/**
 * @brief Creates the needed tables
 * if the table 'packet' already exists but it has a different set of columns
 * of the set that we defined, rename it in 'packet_old' (after dropping
 * whatever was the old 'packet_old') and create again the 'packet' table
 */
bool DbManager::createTables()
{
    /*
    QString sender_mac;
    quint32 timestamp;
    qint8 rssi;
    QString hashed_pkt;
    QString ssid;
    QString espName;
    */
    writeLog("#DbManager");
    QSqlQuery query;
    QString strQuery;
    bool res;


    // not needed
    //strQuery = "DROP TABLE packet";
    //res = query.exec(strQuery);
    //writeLog("TABLE packet DELETED: " + QString::number(res) + "/1" );

    QString strQuery2 = "DROP TABLE packet_old";
    QString strQuery3 = "ALTER TABLE packet RENAME TO packet_old";
    res = query.exec(strQuery2);
    writeLog("if the table 'packet' already exists but it has a different set of columns "
             "of the set that we defined, rename it in 'packet_old' (after dropping "
             "whatever was the old 'packet_old') and create again the 'packet' table" );
    writeLog("TABLE packet_old DELETED: " + QString::number(res) + "/1" );
    res = query.exec(strQuery3);
    writeLog("TABLE packet RENAMED TO packet_old: " + QString::number(res) + "/1" );

    strQuery = "CREATE TABLE packet("
               "id integer primary key,"
               "sender_mac text,"
               "timestamp integer,"
               "rssi integer,"
               "hashed_pkt text,"
               "ssid text,"
               "espName text)";
    res = query.exec(strQuery);
    writeLog("TABLE packet CREATED: " + QString::number(res) + "/1" );

    return res;
}

/**
 * @brief Add a packet in the db
 * ATTENTION TO ALL 9 Y.O. GAMERS
 * Record and Packet are synonymous
 */
bool DbManager::addPacket(Record r)
{
    writeLog("#DbManager");
    QSqlQuery query;
    query.exec("SELECT MAX(id)+1 FROM packet;");
    bool res = query.exec();
    int id = 0;

    query.next();
    //writeLog("POS: " + QString::number(query.at()) );
    //writeLog("MAX(id)+1 calculated: " + QString::number(res) + "/1" );
    id = query.value(0).toInt();

    query.prepare("INSERT INTO packet"
                  "(id,sender_mac,timestamp,rssi,hashed_pkt,ssid,espName) "
                  "VALUES (:id, :sender_mac, :timestamp, :rssi, :hashed_pkt,"
                  ":ssid, :espName)");
    query.bindValue(":id", id);
    query.bindValue(":sender_mac", r.sender_mac);
    query.bindValue(":timestamp", r.timestamp);
    query.bindValue(":rssi", r.rssi);
    query.bindValue(":hashed_pkt", r.hashed_pkt);
    query.bindValue(":ssid", r.ssid);
    query.bindValue(":espName", r.espName);

    res = query.exec();
    writeLog("packet inserted: " + QString::number(res) + "/1" );

    return true;
}

/**
 * @todo implement
 * LAST_TS = the timestamp of the last time we calculated the positions
of devices

"CREATE TABLE packet("
 "id integer primary key,"
 "sender_mac text,"
 "timestamp integer,"
 "rssi integer,"
 "hashed_pkt text,"
 "ssid text,"
 "espName text)"

SELECT 		sender_mac, espName, AVG(rssi) AS avgRssi
FROM 		packet P
WHERE		timestamp > LAST_TS
    AND	hashed_pkt IN (
            SELECT 		hashed_pkt
            FROM 			packet P1
            WHERE 		timestamp > LAST_TS
            GROUP BY	hashed_pkt
            HAVING COUNT(*) = N)
GROUP BY	sender_mac, espName
 */
bool DbManager::calculateAvgRssi(){
    return false;
}

/**
 * @brief Test database functions
 */
void DbManager::test()
{
    //writeLog("#DbManager");
    QSqlQuery query;
    bool res;

    Record r;
    r.sender_mac="HEYNIGGAITSME";
    r.timestamp=123456789;
    r.rssi = -50;
    r.hashed_pkt = "123456";
    r.ssid = "fuori dalla mia wifi N3GR0";
    r.espName = "ESP69";
    addPacket(r);


    res = query.exec("SELECT id, sender_mac FROM packet");
    writeLog("select result: " + QString::number(res) + "/1" );

    while (query.next()) {
        int id = query.value(0).toInt();
        writeLog("id: " + QString::number(id));
        QString sender_mac = query.value(1).toString();
        writeLog("sender_mac: " + sender_mac);  
    }
}

bool DbManager::saveCsv(Record &r, const QString& path)
{
    QFile file(path);

    if(file.open(QFile::WriteOnly | QIODevice::Append)) {
        QTextStream stream(&file);
        stream << r.toString() << endl;
        file.close();
        return true;
    }else{
        return false;
    }
}

void DbManager::setPath(QString p)
{
    db.setDatabaseName(p);
}

