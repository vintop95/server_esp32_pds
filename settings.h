/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#ifndef SETTINGS_H
#define SETTINGS_H

#include "record.h"

#include <QObject>
#include <QSettings>
#include <QSharedPointer>
#include <QDateTime>

static QString SETTINGS_PATH = "settings.ini";

static quint16 SERVER_PORT = 7856;
static int ESP32_NO = 4;
// The application shows the devices in area each CHART_PERIOD ms
static int CHART_PERIOD = 5*1000; //TODO: 5*60*1000;
static QDateTime START_TIME = QDateTime::currentDateTime();
static int ESP32_NO_LIMIT = 15;

/**
 * This class models the settings of the application
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
public:
    QSharedPointer<QList<ESP32>> espList;
    static Settings* getInstance();
    void loadSettings(QSharedPointer<QList<ESP32>>);
    void test();
};

#endif // SETTINGS_H
