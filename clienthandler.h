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
#include "devicefinder.h"
#include <QThread>
#include <QTcpSocket>
#include <QDebug>
#include <QTimer>


/**
 * It handles the data received from the esp32 devices
 */
class ClientHandler : public QObject
{
    Q_OBJECT
public:
   explicit ClientHandler(qintptr s, QObject *parent = nullptr);
   void handle();
   void setMultithread(bool flag){
       writeLog("ClientHandler");
       writeLog("Multithreading set to " + QString::number(flag));
       isMultithread=flag;
   }
signals:
   void error(QTcpSocket::SocketError socketerror);

public slots:
   void readyRead();
   void disconnected();

private:
   DeviceFinder *deviceFinder;
   QTcpSocket *socket;

   qintptr socketDescriptor;
   bool isMultithread = true;
   QString espName = "UNKNOWN";
   QByteArray data;
   QTimer timer;
   int waitPeriod = 40000;


   void setSocketDescriptor(qintptr);
   void pushRecord();
};

#endif // CLIENTHANDLER_H
