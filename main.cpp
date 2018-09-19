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

/**
 * @brief Write into the log window
 *
 * @param Text to write
 * @param Type of log message:
 * - QtDebugMsg:    for debug purposes
 * - QtInfoMsg:     for information purposes
 * - QtWarningMsg:  something went wrong
 * - QtCriticalMsg: something went very wrong
 * - QtFatalMsg:    something went terribly wrong
 */
void writeLog(const QString &text, QtMsgType type){
    pLog->writeLog(text, type);
}

/**
 * @brief Loads settings from the .ini file
 * It generates a .ini file or reads from it
 * It uses <QSettings>
 *
 * @param List of esp32 devices read from .ini file
 */
void loadSettings(QList<ESP32> &espList){
    // Read settings
    QString settingsFile =
            //QApplication::applicationDirPath() + "/" +
            "settings.ini";
    QSettings settings(settingsFile, QSettings::IniFormat);
    QStringList keys = settings.allKeys();

    // If the file does not exist
    if(! settings.contains("ESP32_NO")){
        settings.setValue("CHART_PERIOD", 10000);
        settings.setValue("ESP32_NO", ESP32_NO);

        for(int i=0; i < ESP32_NO; ++i){
            settings.setValue(QString("ESP%1/name").arg(i), QString("ESP%1").arg(i));
            settings.setValue(QString("ESP%1/pos_x").arg(i), 0);
            settings.setValue(QString("ESP%1/pos_y").arg(i), 0);
        }

    }else{
        CHART_PERIOD = settings.value("CHART_PERIOD", 1000).toInt();
        ESP32_NO = settings.value("ESP32_NO").toInt();

        for(int i=0; i < ESP32_NO; ++i){
            QString name = settings.value(QString("ESP%1/name").arg(i), QString("not_found_%1").arg(i)).toString();
            float x = settings.value(QString("ESP%1/pos_x").arg(i), -1).toFloat();
            float y = settings.value(QString("ESP%1/pos_y").arg(i), -1).toFloat();

            espList.push_back(ESP32(name, QPointF(x,y)));
        }
    }
}


int main(int argc, char *argv[])
{
    // Necessary in order to make the program work
    QApplication a(argc, argv);

    // Initializes the main window and open it
    // static pointers are global variables
    MainWindow w;
    pWin = &w;
    Logger logger(pWin);
    pLog = &logger;
    w.show();

    // Retrieve the list of esp32 devices
    QList<ESP32> espList;
    loadSettings(espList);
    writeLog("NUM OF ESPs: " + QString::number(ESP32_NO));


    writeLog("+++ WELCOME TO THE ESP32 SERVER +++", QtInfoMsg);

    // Initializes the object that handles the packets
    // to find the devices in the area
    DeviceFinder deviceFinder(ESP32_NO, CHART_PERIOD);
    deviceFinder.setWindow(&w);
    for(auto e : espList){
        deviceFinder.setESPPos(e.getName(), e.getX(), e.getY());
    }
    deviceFinder.test();

    // Initializes the server that listens for the
    // esp devices to send the packets
    Server c(SERVER_PORT);
    c.setMultithread(false);
    c.start();

    return a.exec();
}
