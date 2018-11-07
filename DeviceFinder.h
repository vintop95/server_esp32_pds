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
    QPointF trilateration(QPointF p1, QPointF p2, QPointF p3, double r1, double r2, double r3);
public:
    DeviceFinder(int espNo, QString dbPath="server_esp32_pds.sqlite3");
    //void run() override;
    void setWindow(MainWindow *);
    void setESPPos(QString ESPName, double xpos, double ypos);
    void pushRecord(Record r);
    void logCurrentDevices();
    int countCurrentDevices();
    void initChart();
    void test();
};

/**
 * Nedded to have a singleton global class
 */
extern DeviceFinder* pDF;

#endif // DEVICEFINDER_H
