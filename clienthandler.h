/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include "devicefinder.h"
#include <QTcpSocket>

/**
 * It handles the data received from the esp32 devices
 */
class ClientHandler : public QObject
{
    Q_OBJECT

private:
   QTcpSocket *socket;
   DeviceFinder *deviceFinder;

   qintptr socketDescriptor;
   QString espName = nullptr;
   QByteArray dataReceived;
   QTimer timeoutDisconnect;
   int timeoutPeriod = 30000;

   void setSocketDescriptor(qintptr socketDescriptor);
   void pushPacketsToDeviceFinder();

signals:
   void error(QTcpSocket::SocketError socketerror);

public slots:
   // CLIENT ESP32
   void readFromSocket();
   void closeClientHandler();

public:
   // SERVER
   explicit ClientHandler(qintptr s, QObject *parent = nullptr);
   void handle();
};

#endif // CLIENTHANDLER_H
