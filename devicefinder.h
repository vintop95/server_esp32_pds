/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#ifndef DEVICEFINDER_H
#define DEVICEFINDER_H

#include "main.h"
#include "dbmanager.h"

/**
 * It detects the devices by processing Records received from the ClientHandler
 * Implement run() method in order to run this on a new thread
 */
class DeviceFinder : public QThread
{
private:
    static DeviceFinder* instance;
    MainWindow* pWin;
    DbManager db;
    QVector<Record> records = QVector<Record>();
    //a vector that holds true in position i if ESP32#i already connected to server during the listening window
    QMap<QString, bool> espInteracted;
    QMap<QString, ESP32> esp32s;
    QHash<QString, Device> devices;

    QTimer timer;
    uint last_ts;


    DeviceFinder();
    void setWindow(MainWindow *);
    void init(QString dbPath="server_esp32_pds.sqlite3");

    void setInteractionWithEsp(QString espName);
    void processLocationsFromPackets();
    void resetInteractionsWithEsp();

    void pushDevice(Device d);    

    QPointF calculatePosition(Record r);
    static QPointF trilateration(QPointF p1, QPointF p2, QPointF p3, double r1, double r2, double r3);
    static std::pair<QPointF, QPointF> bilateration(QPointF p1, QPointF p2, double r1, double r2);

public:
    bool canStartPacketProcessing();

    //void run() override;
    static DeviceFinder* getInstance(
            QString dbPath="server_esp32_pds.sqlite3");
    void initChart();

    void addEsp(QString espName, double xpos, double ypos);

    void pushRecord(Record r);
    void insertPacketsIntoDB();

    static double calculateDistance(int rssi);

    void logCurrentDevices();
    int countCurrentDevices();
    int getEspNo();

    void test();
};

#endif // DEVICEFINDER_H
