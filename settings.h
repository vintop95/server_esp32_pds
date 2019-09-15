/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#ifndef SETTINGS_H
#define SETTINGS_H

#include "datastructures.h"

#include <QObject>
#include <QSettings>
#include <QSharedPointer>
#include <QDateTime>

static QString SETTINGS_PATH = "settings.ini";

static quint16 SERVER_PORT = 7856;
static int ESP32_NO = 4;
// The application shows the devices in area each CHART_PERIOD ms
static int CHART_PERIOD_MS = 60*1000; //TODO: 5*60*1000;
static QDateTime START_TIME = QDateTime::currentDateTime();
static int ESP32_NO_LIMIT_SUP = 15;
static int ESP32_NO_LIMIT_INF = 2;

/**
 * This class models the settings of the application
 * LIFECYCLE OF SETTINGS:
 * At start call loadSettingsFromIni() and use it for the rest of
 * the program
 * If you want to change the settings, change it from the ui
 * and restart, because we do:
 *  loadValuesFromIniToWidget()
 *  saveValuesFromWidgetToIni()
 *
 * Singleton pattern is applied
 */
class Settings : public QObject
{
    Q_OBJECT
private:
    QSettings* qset;
    static Settings* instance;
    Settings();
    ~Settings();
    bool writeSettingsToIni(); //da non usare

    void test();
public:
    // EVERYONE
    espMapPtr_t esp32s = espMapPtr_t::create();
    // MAIN.C
    static Settings* getInstance();
    void loadSettingsFromIni();
};

#endif // SETTINGS_H
