#include "logger.h"

Logger::Logger(MainWindow* w, QObject* parent):QObject(parent), pWin(w){
    connect(this,&Logger::writeLogInMainWindow,pWin,&MainWindow::writeLog);
}

/*
 * We are sure that this call is thread-safe (even if we did not use
 * mutex) because emitting signals in Qt::AutoConnection mode will
 * guarantee mutual exclusion in access of ui elements
 */
void Logger::writeLog(const QString &text, QtMsgType type){
    QString msg = text;

    switch (type) {
        case QtDebugMsg:
            msg = "DEBUG: " + msg;
            msg = "[" + QTime::currentTime().toString() + "] " + msg;
            qDebug().nospace().noquote() << msg;
            break;
        case QtInfoMsg:
            msg = "INFO: " + msg;
            msg = "[" + QTime::currentTime().toString() + "] " + msg;
            qInfo().nospace().noquote() << msg;
            break;
        case QtWarningMsg:
            msg = "WARNING: " + msg;
            msg = "[" + QTime::currentTime().toString() + "] " + msg;
            qWarning().nospace().noquote() << msg;
            break;
        case QtCriticalMsg:
            msg = "CRITICAL: " + msg;
            msg = "[" + QTime::currentTime().toString() + "] " + msg;
            qCritical().nospace().noquote() << msg;
            break;
        case QtFatalMsg:
            msg = "FATAL: " + msg;
            msg = "[" + QTime::currentTime().toString() + "] " + msg;
            emit writeLogInMainWindow(msg);
            qFatal(msg.toUtf8().constData());
            abort();
    }
    emit writeLogInMainWindow(msg);
}
