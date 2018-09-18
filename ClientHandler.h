#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include "main.h"
#include "DeviceFinder.h"
#include <QThread>
#include <QTcpSocket>
#include <QDebug>

/*
 * Use only for time consuming tasks
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

class ClientHandler : public QObject
{
    Q_OBJECT
public:
   explicit ClientHandler(qintptr ID, QObject *parent = nullptr);

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
};

#endif // CLIENTHANDLER_H
