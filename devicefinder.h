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
 * It detects the devices by processing packets received from the ClientHandler
 * Implement run() method in order to run this on a new thread
 */
class DeviceFinder : public QThread
{
private:
    static DeviceFinder* instance;
    MainWindow* pWin;
    DbManager db;
    // A vector that holds true in position i if ESP32#i already connected to
    // server during the listening window
    QMap<QString, bool> espInteracted;
    QVector<Packet> packets;
    QMap<QString, ESP32> esp32s;
    QHash<QString, Device> devices;

    QTimer chartUpdateTimer;
    uint lastTimestamp;

    DeviceFinder();
    void setWindow(MainWindow *);
    void init(QString dbPath="server_esp32_pds.sqlite3");
    void setChartUpdateTimer();

    bool canStartPacketProcessing();
    void setEspInteracted(QString espName);
    void resetInteractionsWithEsp();
    void processLocationsFromPackets();
    void pushDevice(Device d);

    QPointF calculatePosition(Packet r);
    static QPointF trilateration(QPointF p1, QPointF p2, QPointF p3, double r1, double r2, double r3);
    static std::pair<QPointF, QPointF> bilateration(QPointF p1, QPointF p2, double r1, double r2);

public slots:
    // MAINWINDOW
    void logCurrentDevices();

public:
    //void run() override;

    // EVERYONE
    static DeviceFinder* getInstance(
            QString dbPath="server_esp32_pds.sqlite3");
    static double calculateDistance(int rssi);
    int getEspNo();
    int countCurrentDevices();

    // MAIN.C
    void addEsp(QString espName, double xpos, double ypos);

    // CLIENT_HANDLER
    void pushPacket(Packet p);
    bool insertPacketsIntoDB(QString espName);

    void test();
};

#endif // DEVICEFINDER_H
