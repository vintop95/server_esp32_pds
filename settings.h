#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include "logger.h"
#include "record.h"

static quint16 SERVER_PORT = 7856;
static int ESP32_NO = 4;
// The application shows the devices in area each CHART_PERIOD ms
static int CHART_PERIOD = 10000;

class Settings : public QSettings
{
    Q_OBJECT
public:
    Settings();
    void loadSettings(QList<ESP32> &);
};

#endif // SETTINGS_H
