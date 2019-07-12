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

   writeLog("#DbManager");
   if (!db.open())
   {
      writeLog("Connection with database fail", QtCriticalMsg);
   }
   else
   {
      writeLog("connection ok");
   }

   createTablesIfNotExist();

   computeDeviceFrequencies(1562889600, 1562968740);
}

// returns if the table has been correctly created  (if it was needed)
void DbManager::checkConsistencyAndCreateTable(QString TABLE_NAME, int nOfCols, QString strQueryCreate ) {
    // if table does not exist OR numero di colonne previste non corrisponde con il numero di colonne reali
    if ( !db.tables().contains(TABLE_NAME) || db.driver()->record(TABLE_NAME).count() != nOfCols  ) {
        QSqlQuery query;
        QString strQuery2 = "DROP TABLE "+TABLE_NAME+"_old";
        QString strQuery3 = "ALTER TABLE "+TABLE_NAME+" RENAME TO "+TABLE_NAME+"_old";
        bool res = query.exec(strQuery2);
        writeLog("if the table '"+TABLE_NAME+"' already exists but it has a different set of columns "
                 "of the set that we defined, rename it in '"+TABLE_NAME+"_old' (after dropping "
                 "whatever was the old '"+TABLE_NAME+"_old') and create again the '"+TABLE_NAME+"' table" );
        writeLog("TABLE "+TABLE_NAME+"_old DELETED: " + QString::number(res) + "/1" );
        res = query.exec(strQuery3);
        writeLog("TABLE "+TABLE_NAME+" RENAMED TO "+TABLE_NAME+"_old: " + QString::number(res) + "/1" );


        res = query.exec(strQueryCreate);
        writeLog("TABLE "+TABLE_NAME+" CREATED: " + QString::number(res) + "/1" );

        if (!res) {
            throw std::runtime_error("FAIL TO CREATE TABLE");
        }
    }
}
/**
 * @brief Creates the needed tables
 * if the table 'packet' already exists but it has a different set of columns
 * of the set that we defined, rename it in 'packet_old' (after dropping
 * whatever was the old 'packet_old') and create again the 'packet' table
 */
void DbManager::createTablesIfNotExist()
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

    QString strQuery;

    int nOfCols;
    QString TABLE_NAME;

    // TABELLA 1: PACCHETTI NON ELABORATI
    TABLE_NAME = "packet";
    nOfCols=7;
    strQuery = "CREATE TABLE "+TABLE_NAME+"("
               "id integer primary key,"
               "sender_mac text,"
               "timestamp integer,"
               "rssi integer,"
               "hashed_pkt text,"
               "ssid text,"
               "espName text)";
    checkConsistencyAndCreateTable(TABLE_NAME, nOfCols, strQuery);


    // TABELLA 2: DEVICE TROVATI NEL TEMPO
    TABLE_NAME = "device_position_in_time";
    nOfCols=5;
    strQuery = "CREATE TABLE "+TABLE_NAME+"("
               "id integer primary key,"
               "sender_mac text,"
               "timestamp integer,"
               "x real,"
               "y real)";
    checkConsistencyAndCreateTable(TABLE_NAME, nOfCols, strQuery);
}

/**
 * @brief Add a device in the db
 */
bool DbManager::addDevice(Device d)
{
    QString TABLE_NAME = "device_position_in_time";
    writeLog("#DbManager");
    QSqlQuery query;
    query.prepare("SELECT MAX(id)+1 FROM "+TABLE_NAME+";");
    bool res = query.exec();
    int id = 0;

    query.next();
    //writeLog("POS: " + QString::number(query.at()) );
    //writeLog("MAX(id)+1 calculated: " + QString::number(res) + "/1" );
    id = query.value(0).toInt();

    query.prepare("INSERT INTO " + TABLE_NAME +
                  "(id,sender_mac,timestamp,x,y) "
                  "VALUES (:id, :sender_mac, :timestamp, :x, :y)");
    query.bindValue(":id", id);
    query.bindValue(":sender_mac", d.sender_mac);
    query.bindValue(":timestamp", d.timestamp);
    query.bindValue(":x", d.pos.x());
    query.bindValue(":y", d.pos.y());

    res = query.exec();
    writeLog("Device inserted: " + QString::number(res) + "/1" );

    return res;
}

// AGGIUNGE PIÙ DEVICE CONTEMPORANEAMENTE
bool DbManager::insertDevices(QList<Device> devices)
{
    if(devices.size() > 0){
        writeLog("#DbManager");
        QSqlQuery query;
        QString TABLE_NAME = "device_position_in_time";
        query.prepare("SELECT MAX(id)+1 FROM "+TABLE_NAME+";");
        bool res = query.exec();
        int id = 0;
        query.next();
        //writeLog("POS: " + QString::number(query.at()) );
        //writeLog("MAX(id)+1 calculated: " + QString::number(res) + "/1" );
        id = query.value(0).toInt();
        //Load all packets using a single query
        QString queryString = "INSERT INTO " + TABLE_NAME +
                               "(id,sender_mac,timestamp,x,y) "
                              "VALUES ";
        for(auto device: devices){
            queryString += "("+ QString::number(id++) + ","
                             + "'" + device.sender_mac + "',"
                             + QString::number(device.timestamp) + ","
                             + QString::number(device.pos.x()) + ","
                             + QString::number(device.pos.y()) + "),";
        }
        //rimuoviamo l'ultimo carattere, cioè ',' , e aggiungiamo un ';'
        queryString.chop(1);
        queryString += ";";
        query.prepare(queryString);
        res = query.exec();
        writeLog(QString::number(devices.size()) + " devices inserted: " + QString(res ? "yes" : "no") );
        return res;
    }
    return false;
}

// AGGIUNGE PIÙ PACCHETTI CONTEMPORANEAMENTE
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
              " ORDER BY    sender_mac, avgRssi DESC;";

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

QList<DeviceFrequencyInWindow> DbManager::computeDeviceFrequencies(
        int start_window, int end_window){
    QList<DeviceFrequencyInWindow> deviceFrequencies;

    QSqlQuery query;
    //we execute the following query
    QString queryStr = " SELECT  sender_mac, COUNT(*) AS frequency, "
                       "MIN(timestamp) AS start_subwindow, MAX(timestamp) AS end_subwindow"
              " FROM    device_position_in_time D "
              " WHERE   timestamp >= "+QString::number(start_window)+
              " AND timestamp <= "+QString::number(end_window)+" "
              " GROUP BY sender_mac "
              " ORDER BY COUNT(*) DESC; ";

    writeLog(queryStr, QtWarningMsg);
    bool success = query.exec(queryStr);

    if(!success){
        throw std::runtime_error("Query computeDeviceFrequencies FAILED!");
    }
    while (query.next()) {
        DeviceFrequencyInWindow devFreq;
        devFreq.sender_mac = query.value("sender_mac").toString();
        devFreq.frequency = query.value("frequency").toInt();
        devFreq.start_subwindow = query.value("start_subwindow").toInt();
        devFreq.end_subwindow = query.value("end_subwindow").toInt();

        writeLog("IL DISPOSITIVO "+ devFreq.sender_mac
                 + " appare " + QString::number(devFreq.frequency)+ " volte. "
                 + " finestra:( " + QString::number(devFreq.start_subwindow) + ", "
                                  + QString::number(devFreq.end_subwindow) + ")"
                 , QtWarningMsg);

       deviceFrequencies.push_back(devFreq);
    }

    return deviceFrequencies;
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

