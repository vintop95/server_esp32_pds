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
    // A vector that holds true in position i if ESP32#i
    // already connected to server during the listening window
    QMap<QString, bool> espInteracted;
    QVector<Packet> packets;
    espMapPtr_t esp32s;
    QMap<QString, Device> devices;

    QTimer chartUpdateTimer;
    uint lastTimestamp;

    DeviceFinder();
    void setWindow(MainWindow *);
    void init(espMapPtr_t list = nullptr,
              QString dbPath="server_esp32_pds.sqlite3");
    void setChartUpdateTimer();
    bool isAllowedESPName(QString name);
    bool canStartPacketProcessing();
    void setEspInteracted(QString espName);
    void resetInteractionsWithEsp();
    void processLocationsFromPackets();
    void pushDevice(Device d);

    static QPointF trilateration(QPointF p1, QPointF p2, QPointF p3,
                                 double r1, double r2, double r3);
    static QPair<QPointF, QPointF> bilateration(
            QPointF p1, QPointF p2, double r1, double r2);
    //viene chiamato dentro init
    void test();


public slots:
    // MAINWINDOW
    void logCurrentDevices();

public:
    //void run() override;

    // EVERYONE
    static DeviceFinder* getInstance(espMapPtr_t list = nullptr,
            QString dbPath="server_esp32_pds.sqlite3");
    static double calculateDistance(double rssi);
    int getEspNo();
    int countCurrentDevices();

    // CLIENT_HANDLER
    void pushPacket(Packet p);
    bool insertPacketsIntoDB(QString espName);


};

#endif // DEVICEFINDER_H
