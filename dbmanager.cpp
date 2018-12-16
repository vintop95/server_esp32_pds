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
       QSqlRecord cols = db.driver()->record("packet");
       if (cols.count() != nOfCols){
            createTables();
       };
   }
   //TODO: da togliere
   //lastTimestamp = QDateTime::currentDateTime().toTime_t();
   //test();
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
 */
bool DbManager::addPacket(Packet p)
{
    writeLog("#DbManager");
    QSqlQuery query;
    query.prepare("SELECT MAX(id)+1 FROM packet;");
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
    query.bindValue(":sender_mac", p.sender_mac);
    query.bindValue(":timestamp", p.timestamp);
    query.bindValue(":rssi", p.rssi);
    query.bindValue(":hashed_pkt", p.hashed_pkt);
    query.bindValue(":ssid", p.ssid);
    query.bindValue(":espName", p.espName);

    res = query.exec();
    writeLog("Packet inserted: " + QString::number(res) + "/1" );

    return res;
}

bool DbManager::insertPackets(QVector<Packet> packets)
{
    if(packets.size() > 0){
        writeLog("#DbManager");
        QSqlQuery query;
        query.prepare("SELECT MAX(id)+1 FROM packet;");
        bool res = query.exec();
        int id = 0;
        query.next();
        //writeLog("POS: " + QString::number(query.at()) );
        //writeLog("MAX(id)+1 calculated: " + QString::number(res) + "/1" );
        id = query.value(0).toInt();
        //Load all packets using a single query
        QString queryString = "INSERT INTO packet"
                              "(id,sender_mac,timestamp,rssi,hashed_pkt,ssid,espName) "
                              "VALUES ";
        for(auto packet: packets){
            queryString += "("+ QString::number(id++) + ","
                                     + "'" + packet.sender_mac + "',"
                                     + QString::number(packet.timestamp) + ","
                                     + QString::number(packet.rssi) +","
                                     + "'" + packet.hashed_pkt +"',"
                                     + "'" + packet.ssid +"',"
                                     + "'" + packet.espName + "'),";
        }
        //rimuoviamo l'ultimo carattere, cioè ',' , e aggiungiamo un ';'
        queryString.chop(1);
        queryString += ";";
        query.prepare(queryString);
        res = query.exec();
        writeLog(QString::number(packets.size()) + " packets inserted: " + QString(res ? "yes" : "no") );
        return res;
    }
    return false;
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
//returns a map with the avg rssi for each device (see datastructures.h)
//params: total number of ESP in the system, beginning of the time window in which we do the analysis
avgRssiMap_t DbManager::calculateAvgRssi(int espNumber, unsigned int lastTimestamp){
    avgRssiMap_t avgRssiMap;
    QSqlQuery query;
    //we execute the following query
    QString queryStr = " SELECT    sender_mac, espName, AVG(rssi) AS avgRssi"
              " FROM 		packet P "
              " WHERE		timestamp > " + QString::number(lastTimestamp) +
              " AND  hashed_pkt IN ("
                          "  SELECT 		hashed_pkt "
                          "  FROM 		packet P1"
                          "  WHERE 		timestamp >" + QString::number(lastTimestamp) +
                          "  GROUP BY	hashed_pkt"
                          "  HAVING COUNT(*) ="+QString::number(espNumber) +")"
              " GROUP BY	sender_mac, espName"
              " ORDER BY    sender_mac, avgRssi;";

    writeLog(queryStr, QtWarningMsg);
    bool success = query.exec(queryStr);

    if(!success){
        throw std::runtime_error("Query to calculate average RSSI FAILED!");
    }
    while (query.next()) {
        QString deviceMac = query.value("sender_mac").toString();
        QString ESPName = query.value("espName").toString();
        double avgRssi = query.value("avgRssi").toDouble();
        writeLog("IL DISPOSITIVO "+ deviceMac +" RILEVATO DA " + ESPName
                 + " CON UN RSSI MEDIO DI " + QString::number(avgRssi), QtWarningMsg);
        //riempiamo la mappa

       //aggiungiamo nel vector relativo a quel dispositivo
       //una nuova entry contenente la coppia <Schedina, RssiMedio>
       avgRssiMap[deviceMac].push_back(QPair<QString, double>(ESPName, avgRssi));
    }

    return avgRssiMap;

}

/**
 * @brief Test database functions
 */
void DbManager::test()
{
    //writeLog("#DbManager");
    QSqlQuery query;
    bool res;

    Packet r;
    r.sender_mac="HEYNIGGAITSME";
    r.timestamp=123456789;
    r.rssi = -50;
    r.hashed_pkt = "123456";
    r.ssid = "fuori dalla mia wifi N3GR0";
    r.espName = "ESP69";
    writeLog("TEST: add packet");
    addPacket(r);


    res = query.exec("SELECT id, sender_mac FROM packet");
    writeLog("TEST: select result: " + QString::number(res) + "/1" );

    while (query.next()) {
        int id = query.value(0).toInt();
        writeLog("id: " + QString::number(id));
        QString sender_mac = query.value(1).toString();
        writeLog("sender_mac: " + sender_mac);  
    }
}

void DbManager::test_2()
{
    writeLog("#DbManager TEST 2");
    QSqlQuery query;
    bool res;

    Packet r;
   // writeLog("TEST: read packets from db since " + QString::number(lastTimestamp) );



    res = query.exec("SELECT "
                     "id, "
                     "sender_mac, "
                     "timestamp, "
                     "rssi, "
                     "hashed_pkt, "
                     "ssid, "
     //                "espName FROM packet WHERE timestamp > "+ QString::number(lastTimestamp) +
                     " ORDER BY sender_mac, timestamp, hashed_pkt;");
    writeLog("TEST: select result: " + QString::number(res) + "/1" );

    while (query.next()) {
        r.sender_mac = query.value(1).toString();
        r.timestamp = query.value(2).toUInt();
        r.rssi = static_cast<qint8>(query.value(3).toInt());
        r.hashed_pkt = query.value(4).toString();
        r.ssid = query.value(5).toString();
        r.espName = query.value(6).toString();
        writeLog("PACCHETTO "+ query.value(0).toString() +" LETTO: " + r.toString() );
    }

 /*   res = query.exec("SELECT hashed_pkt, sender_mac, COUNT(*) FROM Packet GROUP BY hashed_pkt, sender_mac HAVING COUNT(*) > 1;");
    writeLog("TEST: select result: " + QString::number(res) + "/1" );
    while (query.next()) {
        writeLog("HASH PACCHETTO: "+ query.value(0).toString() +" MAC: "+ query.value(1).toString() + " RICEVUTO: " + query.value(2).toString() + " VOLTE" );
    } */
    int N=2;
    //query per ritornare i mac dei dispositivi rilevati, le schedine che li hanno rilevati
    //e l'RSSI medio nell'ultimo intervallo temporale
    QString prova = " SELECT    sender_mac, espName, AVG(rssi) AS avgRssi"
              " FROM 		packet P "
       //       " WHERE		timestamp > " + QString::number(lastTimestamp) +
              " AND  hashed_pkt IN ("
                          "  SELECT 		hashed_pkt "
                          "  FROM 		packet P1"
         //                 "  WHERE 		timestamp >" + QString::number(lastTimestamp) +
                          "  GROUP BY	hashed_pkt"
                         //TODO:
                          "  HAVING COUNT(*) ="+QString::number(N) +")"
              " GROUP BY	sender_mac, espName;";
    writeLog(prova, QtWarningMsg);
    res = query.exec(prova);
    writeLog("TEST: select result: " + QString::number(res) + "/1" );
    while (query.next()) {
        QString device = query.value("sender_mac").toString();
        QString ESPName = query.value("espName").toString();
        double avgRssi = query.value("avgRssi").toDouble();
        writeLog("IL DISPOSITIVO "+ device +" RILEVATO DA " + ESPName + " CON UN RSSI MEDIO DI " + QString::number(avgRssi), QtWarningMsg);
    }
    //TODO: DA TOGLIERE
   // lastTimestamp = QDateTime::currentDateTime().toTime_t();
}

void DbManager::setPath(QString p)
{
    db.setDatabaseName(p);
}

