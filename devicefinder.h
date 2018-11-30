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
#include <QVector>
#include <QtMath>

#include "dbmanager.h"
#include "record.h"

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
    int ESPNo;
    QVector<Record> records = QVector<Record>();
    //a vector that holds true in position i if ESP32#i already connected to server during the listening window
    QMap<QString, bool> ESPInteracted;
    QMap<QString, ESP32> esp;
    QHash<QString, Device> devices;

    QTimer timer;
    uint last_ts;


    DeviceFinder();
    void pushDevice(Device d);
    void setWindow(MainWindow *);
    void resetContactsMap();

    QPointF calculatePosition(Record r);
    static QPointF trilateration(QPointF p1, QPointF p2, QPointF p3, double r1, double r2, double r3);
    static std::pair<QPointF, QPointF> bilateration(QPointF p1, QPointF p2, double r1, double r2);

public:
    static DeviceFinder* getInstance(int espNo = 0, QString dbPath="server_esp32_pds.sqlite3");

    //void run() override;
    void setInit(int espNo, QString dbPath="server_esp32_pds.sqlite3");

    void setESPPos(QString ESPName, double xpos, double ypos);
    void pushRecord(Record r);
    void addPacketsToDB();
    bool canStartProcessing();
    void setContactedByID(QString ESPid);
    void processData();
    static double calculateDistance(int rssi);
    void logCurrentDevices();
    int countCurrentDevices();
    void initChart();
    void test();
};

#endif // DEVICEFINDER_H
