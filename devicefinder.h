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

    QSet<QString> devices_in_window;


    QVector<Device> hiddenDevices;
    /*
     * CORRELATION MATRIX BETWEEN HIDDEN DEVICES
     *    M1  M2    M3
     * M1 1   0.6  0.8
     * M2      1   0.8
     * M3           1
     */
    QVector< QVector<int> > hiddenMacCorrelationMatrix;

    QTimer chartUpdateTimer;
    uint lastTimestamp;
    bool hasTimerReset = true;

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

    //DEVICE MANAGEMENT
    void pushDeviceInBuffer(Device d);
    bool insertBufferedDevicesIntoDB();
    void updateDevicesInWindow();

    static QPointF trilateration(QPointF p1, QPointF p2, QPointF p3,
                                 double r1, double r2, double r3);
    static QPointF bilateration(
            QPointF p1, QPointF p2, double r1, double r2);
    static QPair<QPointF, QPointF> bilaterationThatReturnsTwoPoints(
            QPointF p1, QPointF p2, double r1, double r2);



    void hiddenMacRecognition();
    static double correlation(Device d1, Device d2);
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
    void pushPacketInBuffer(Packet p);
    bool insertBufferedPacketsIntoDB(QString espName);

    //viene chiamato dentro init
    void generatePackets();

    DbManager* getDb();
};

#endif // DEVICEFINDER_H
