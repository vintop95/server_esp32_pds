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

    Settings* settings = Settings::getInstance();
    settings->loadSettingsFromIni();

    MainWindow* w = MainWindow::getInstance();
    w->show();

    writeLog("+++ WELCOME TO THE ESP32 SERVER +++", QtInfoMsg);

    printMainRssiMeasures();

    // Initializes the object that handles the packets
    // to find the devices in the area
    DeviceFinder::getInstance(settings->esp32s);
    // Initializes the server that listens for the
    // esp devices to send the packets
    Server* s = Server::getInstance(SERVER_PORT);
    s->start();


    return a.exec();
}
