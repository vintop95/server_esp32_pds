/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#include "settings.h"
#include <QMessageBox>
#include <QtGlobal>
#include <QDebug>
#include <QCoreApplication>

Settings* Settings::instance;

/**
 * @brief Constructor of Settings
 */
Settings::Settings()
{
    qset = new QSettings(SETTINGS_PATH,QSettings::IniFormat);
}

Settings::~Settings()
{
    delete qset;
}

Settings* Settings::getInstance()
{
    if (instance == nullptr){
        instance = new Settings();
    }
    return instance;
}

/**
 * @brief Loads settings from the .ini file
 * It generates a .ini file or reads from it
 * It uses <QSettings>
 *
 * @param List of esp32 devices read from .ini file
 */
void Settings::loadSettingsFromIni(){
    qset->setPath(QSettings::IniFormat, QSettings::UserScope, SETTINGS_PATH);
    qset->sync();

    // If the file does not exist
    if(! qset->contains("ESP32_NO")){
        //qDebug() << qset->applicationName();

        qset->setValue("CHART_PERIOD", CHART_PERIOD_MS);
        qset->setValue("ESP32_NO", ESP32_NO);

        for(int i=0; i < ESP32_NO; ++i){
            QString name = QString("ESP%1").arg(i);
            qreal x = 0.0;
            qreal y = 0.0;

            qset->setValue(QString("ESP%1/name").arg(i), name);
            qset->setValue(QString("ESP%1/pos_x").arg(i), QString::number(x,'f'));
            qset->setValue(QString("ESP%1/pos_y").arg(i), QString::number(y,'f'));

            esp32s->insert(name,ESP32(name, QPointF(x,y)));

            qset->sync();
        }

    }else{
        CHART_PERIOD_MS = qset->value("CHART_PERIOD", 1000).toInt();
        ESP32_NO = qset->value("ESP32_NO").toInt();

        for(int i=0; i < ESP32_NO; ++i){
            QString name = qset->value(QString("ESP%1/name").arg(i), QString("not_found_%1").arg(i)).toString();
            qreal x = qset->value(QString("ESP%1/pos_x").arg(i), -1).toReal();
            qreal y = qset->value(QString("ESP%1/pos_y").arg(i), -1).toReal();

            esp32s->insert(name,ESP32(name, QPointF(x,y)));
            qset->sync();
        }
    }

}

bool Settings::writeSettingsToIni()
{
    //TODO: check if ok
    try{
        qset->setValue("CHART_PERIOD", CHART_PERIOD_MS);
        qset->setValue("ESP32_NO", ESP32_NO);

        for (auto const& esp32 : *esp32s){
            QString name = esp32.getName();
            double x = esp32.getX();
            double y = esp32.getY();

            qset->setValue(QString(name + "/name"), name);
            qset->setValue(QString(name + "/pos_x"), QString::number(x,'f'));
            qset->setValue(QString(name + "/pos_y"), QString::number(y,'f'));
        }
        qset->sync();
        return true;
    }catch(const std::exception& ex){
        return false;
    }

}
