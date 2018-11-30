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

void printRssiDistanceLog(int rssi){
    writeLog("rssi: " + QString::number(rssi) + " dBm, meters: " + QString::number(DeviceFinder::calculateDistance(rssi),'f',2) + " m", QtInfoMsg);
}

void printMainRssiMeasures(){

    writeLog("+++ MAIN RSSI MEASURES IN METERS +++", QtInfoMsg);

    for(int i=-90; i <= -65; i+=1){
        printRssiDistanceLog(i);
    }
    for(int i=-65; i <= -25; i+=5){
        printRssiDistanceLog(i);
    }

    writeLog("++++++++++++++++++++++++++++++++++++", QtInfoMsg);
}

int main(int argc, char *argv[])
{
    // Necessary in order to make the program work
    QApplication a(argc, argv);
    //writeLog("SIZE OF UINT: " + QString::number(sizeof(uint)));
    Settings* settings = Settings::getInstance();
    // Retrieve the list of esp32 devices
    QSharedPointer<QList<ESP32>> espList = QSharedPointer<QList<ESP32>>::create();
    settings->loadSettings(espList);
    ESP32_NO = espList->size();
    writeLog("NUM OF ESPs: " + QString::number(ESP32_NO));

    MainWindow* w = MainWindow::getInstance();
    w->show();

    writeLog("+++ WELCOME TO THE ESP32 SERVER +++", QtInfoMsg);

    printMainRssiMeasures();


    // Initializes the object that handles the packets
    // to find the devices in the area
    DeviceFinder* deviceFinder = DeviceFinder::getInstance();
    for(auto e : *espList){
        deviceFinder->addEsp(e.getName(), e.getX(), e.getY());
    }
    deviceFinder->test();

    // Initializes the server that listens for the
    // esp devices to send the packets
    Server* s = Server::getInstance(SERVER_PORT);
    s->start();


    return a.exec();
}
