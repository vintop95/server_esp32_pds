/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#include "logger.h"

Logger* Logger::instance;

/**
 * @brief Constructor of logger
 */
Logger::Logger( QObject* parent): QObject(parent){
    pWin = MainWindow::getInstance();
    connect(this, &Logger::writeLogInMainWindow,
            pWin, &MainWindow::writeLogInUi);
}

Logger *Logger::getInstance()
{
    if (instance == nullptr){
        instance = new Logger();
    }
    return instance;
}

bool Logger::saveCsv(Packet &r, const QString &path)
{
    QFile file(path);

    if(file.open(QFile::WriteOnly | QIODevice::Append)) {
        QTextStream stream(&file);
        stream << r.toString() << endl;
        file.close();
        return true;
    }else{
        return false;
    }
}

/**
 * @brief Writes the log message in the correct way depending of the type
 * We are sure that this call is thread-safe (even if we did not use
 * mutex) because emitting signals in Qt::AutoConnection mode will
 * guarantee mutual exclusion in access of ui elements
 *
 * @param Text to write
 * @param Type of log message:
 * - QtDebugMsg:    for debug purposes
 * - QtInfoMsg:     for information purposes
 * - QtWarningMsg:  something went wrong
 * - QtCriticalMsg: something went very wrong
 * - QtFatalMsg:    something went terribly wrong
 */
void Logger::writeLog(const QString &text, QtMsgType type){
    QString msg = text;
    QColor txtColor = Qt::white;

    switch (type) {
        case QtDebugMsg:
            msg = "DEBUG: " + msg;
            msg = "[" + QTime::currentTime().toString() + "] " + msg;
            qDebug().nospace().noquote() << msg;
            txtColor = Qt::green;
            break;
        case QtInfoMsg:
            msg = "INFO: " + msg;
            msg = "[" + QTime::currentTime().toString() + "] " + msg;
            qInfo().nospace().noquote() << msg;
            txtColor = Qt::cyan;
            break;
        case QtWarningMsg:
            msg = "WARNING: " + msg;
            msg = "[" + QTime::currentTime().toString() + "] " + msg;
            qWarning().nospace().noquote() << msg;
            txtColor = Qt::yellow;
            break;
        case QtCriticalMsg:
            msg = "CRITICAL: " + msg;
            msg = "[" + QTime::currentTime().toString() + "] " + msg;
            qCritical().nospace().noquote() << msg;
            txtColor = Qt::red;
            break;
        case QtFatalMsg:
            msg = "FATAL: " + msg;
            msg = "[" + QTime::currentTime().toString() + "] " + msg;
            emit writeLogInMainWindow(msg, Qt::red);
            qFatal(msg.toUtf8().constData());
            abort();
    }
    emit writeLogInMainWindow(msg, txtColor);
}

/**
 * @brief Write into the log window, this function is callable
 * from anywhere logger.h is included
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
    Logger::getInstance()->writeLog(text, type);
}
