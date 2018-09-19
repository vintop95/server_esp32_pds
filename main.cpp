#include "main.h"

#include "Server.h"
#include <QPointF>

void writeLog(const QString &text, QtMsgType type){
    pLog->writeLog(text,type);
}

void loadSettings(QList<ESP32> &espList){
    // Read settings
    QString settingsFile =
            //QApplication::applicationDirPath() + "/" +
            "settings.ini";
    QSettings settings(settingsFile, QSettings::IniFormat);
    QStringList keys = settings.allKeys();

    // If the file does not exist
    if(! settings.contains("ESP32_NO")){
        settings.setValue("ESP32_NO", ESP32_NO);

        for(int i=0; i < ESP32_NO; ++i){
            settings.setValue(QString("ESP%1/name").arg(i), QString("ESP%1").arg(i));
            settings.setValue(QString("ESP%1/pos_x").arg(i), 0);
            settings.setValue(QString("ESP%1/pos_y").arg(i), 0);
        }

    }else{
        ESP32_NO = settings.value("ESP32_NO").toInt();

        for(int i=0; i < ESP32_NO; ++i){
            QString name = settings.value(QString("ESP%1/name").arg(i), QString("not_found_%1").arg(i)).toString();
            float x = settings.value(QString("ESP%1/pos_x").arg(i), -1).toFloat();
            float y = settings.value(QString("ESP%1/pos_y").arg(i), -1).toFloat();

            espList.push_back(ESP32(name, QPointF(x,y)));
        }
    }
}

// -35 MOLTO VICINO, -100 MOLTO LONTANO
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    pWin = &w;
    Logger logger(pWin);
    pLog = &logger;
    w.show();

    QList<ESP32> espList;
    loadSettings(espList);

    writeLog("ESP_NO: " + QString::number(ESP32_NO));


    writeLog("+++ WELCOME TO THE ESP32 SERVER +++", QtInfoMsg);

    DeviceFinder deviceFinder(ESP32_NO);
    for(auto e : espList){
        deviceFinder.setESPPos(e.getName(), e.getX(), e.getY());
    }

    deviceFinder.test();

    Server c(SERVER_PORT);
    c.setMultithread(false);
    c.start();

    return a.exec();
}
