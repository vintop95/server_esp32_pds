/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#include "main.h"

#include "Server.h"
#include "DeviceFinder.h"

int main(int argc, char *argv[])
{
    // Necessary in order to make the program work
    QApplication a(argc, argv);

    // Initializes the main window and open it
    // static pointers are global variables
    Settings settings;
    pSet = &settings;
    MainWindow w;
    pWin = &w;
    Logger logger(pWin);
    pLog = &logger;
    w.show();

    // Retrieve the list of esp32 devices
    QSharedPointer<QList<ESP32>> espList = QSharedPointer<QList<ESP32>>::create();
    settings.loadSettings(espList);
    writeLog("NUM OF ESPs: " + QString::number(ESP32_NO));

    w.initAreaChart();

    writeLog("+++ WELCOME TO THE ESP32 SERVER +++", QtInfoMsg);

    // Initializes the object that handles the packets
    // to find the devices in the area
    DeviceFinder deviceFinder(ESP32_NO);
    deviceFinder.setWindow(&w);
    for(auto e : *espList){
        deviceFinder.setESPPos(e.getName(), e.getX(), e.getY());
    }
    deviceFinder.test();

    // Initializes the server that listens for the
    // esp devices to send the packets
    Server s(SERVER_PORT, &deviceFinder);
    s.setMultithread(false);
    s.start();

    return a.exec();
}
