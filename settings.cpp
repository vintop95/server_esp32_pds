#include "settings.h"

Settings::Settings()
{
    QString settingsFile =
            //QApplication::applicationDirPath() + "/" +
            "settings.ini";
    setPath(QSettings::IniFormat, UserScope, settingsFile);
}

/**
 * @brief Loads settings from the .ini file
 * It generates a .ini file or reads from it
 * It uses <QSettings>
 *
 * @param List of esp32 devices read from .ini file
 */
void Settings::loadSettings(QList<ESP32> &espList){

    QStringList keys = allKeys();

    // If the file does not exist
    if(! contains("ESP32_NO")){
        setValue("CHART_PERIOD", 10000);
        setValue("ESP32_NO", ESP32_NO);

        for(int i=0; i < ESP32_NO; ++i){
            setValue(QString("ESP%1/name").arg(i), QString("ESP%1").arg(i));
            setValue(QString("ESP%1/pos_x").arg(i), 0);
            setValue(QString("ESP%1/pos_y").arg(i), 0);
        }

    }else{
        CHART_PERIOD = value("CHART_PERIOD", 1000).toInt();
        ESP32_NO = value("ESP32_NO").toInt();

        for(int i=0; i < ESP32_NO; ++i){
            QString name = value(QString("ESP%1/name").arg(i), QString("not_found_%1").arg(i)).toString();
            float x = value(QString("ESP%1/pos_x").arg(i), -1).toFloat();
            float y = value(QString("ESP%1/pos_y").arg(i), -1).toFloat();

            espList.push_back(ESP32(name, QPointF(x,y)));
        }
    }
}
