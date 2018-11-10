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
    int ESPNo;
    int chartPeriod;
    QVector<Record> records = QVector<Record>();
    QMap<QString, ESP32> esp;
    QHash<QString, Device> devices;
    QTimer timer;
    MainWindow *win;
    DbManager db;

    void pushDevice(Device d);
    QPointF calculatePosition(Record r);
public:
    DeviceFinder(int espNo, int chartPeriod, QString dbPath="server_esp32_pds.sqlite3");
    //void run() override;
    void setWindow(MainWindow *);
    void setESPPos(QString ESPName, float xpos, float ypos);
    void pushRecord(Record r);
    void logCurrentDevices();
    int countCurrentDevices();
    void initChart();
    void test();
};

#endif // DEVICEFINDER_H
