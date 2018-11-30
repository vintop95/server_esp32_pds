/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#ifndef LOGGER_H
#define LOGGER_H

#include "mainwindow.h"

/**
 * We use a Logger class in order to make writeLog a thread-safe
 * function by using signals to call ui with pWin, because
 * calling ui methods from non-main thread is not allowed
 */
class Logger : public QObject{
    Q_OBJECT
private:
    static Logger* instance;
    MainWindow* pWin;
    Logger(QObject* parent = nullptr);

signals:
    void writeLogInMainWindow(const QString &text, const QColor& = Qt::black);

public slots:
    void writeLog(const QString &text, QtMsgType type = QtDebugMsg);

public:
    static Logger* getInstance();
    static bool saveCsv(Packet& p, const QString& path = "output.txt");
};

void writeLog(const QString &text, QtMsgType type = QtDebugMsg);

#endif // LOGGER_H
