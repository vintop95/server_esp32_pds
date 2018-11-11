/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#include "main.h"

#include "server.h"
#include "devicefinder.h"

int main(int argc, char *argv[])
{
    // Necessary in order to make the program work
    QApplication a(argc, argv);

    Settings* settings = Settings::getInstance();

    // Retrieve the list of esp32 devices
    QSharedPointer<QList<ESP32>> espList = QSharedPointer<QList<ESP32>>::create();
    settings->loadSettings(espList);
    ESP32_NO = espList->size();
    writeLog("NUM OF ESPs: " + QString::number(ESP32_NO));

    MainWindow* w = MainWindow::getInstance();
    w->initAreaChart();
    w->show();

    writeLog("+++ WELCOME TO THE ESP32 SERVER +++", QtInfoMsg);

    // Initializes the object that handles the packets
    // to find the devices in the area
    DeviceFinder* deviceFinder = DeviceFinder::getInstance(ESP32_NO);
    for(auto e : *espList){
        deviceFinder->setESPPos(e.getName(), e.getX(), e.getY());
    }
    deviceFinder->test();

    // Initializes the server that listens for the
    // esp devices to send the packets
    Server* s = Server::getInstance(SERVER_PORT);
    s->setMultithread(false);
    s->start();

    return a.exec();
}
