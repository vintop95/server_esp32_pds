/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include "main.h"
#include "DeviceFinder.h"
#include <QThread>
#include <QTcpSocket>
#include <QDebug>

/**
 * Use only for time consuming tasks
 * Use later
 */
class Handle : public QObject, public QRunnable
{
    Q_OBJECT
public:
    qintptr descr;
    Handle(qintptr d): descr(d){

    }
    void run() override{
    }
};

/**
 * It handles the data received from the esp32 devices
 */
class ClientHandler : public QObject
{
    Q_OBJECT
public:
   explicit ClientHandler(qintptr ID, DeviceFinder* dF, QObject *parent = nullptr);

   void handle();
   void setMultithread(bool flag){
       writeLog("Multithreading set to " + QString::number(flag));
       isMultithread=flag;
   }
signals:
   void error(QTcpSocket::SocketError socketerror);

public slots:
   void readyRead();
   void disconnected();

private:
   QTcpSocket *socket;
   DeviceFinder *deviceFinder;
   qintptr socketDescriptor;
   bool isMultithread = true;
   QString espName = "UNKNOWN";
};

#endif // CLIENTHANDLER_H
