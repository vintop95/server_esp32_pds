/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#ifndef SERVER_H
#define SERVER_H

#include "main.h"
#include "ClientHandler.h"
#include <QThreadPool>

/**
 * This class models the TCP Server that receives
 * packets from the ESP32 devices
 */
class Server : public QTcpServer
{
    Q_OBJECT
private:
    quint16 port;
    QTcpSocket *socket;
    QThreadPool *pool;
    bool isMultithread = true;
signals:

public slots:

protected:
    void incomingConnection(qintptr socketDescriptor) override;

public:
    Server(quint16 port, QObject* parent = nullptr);
    void setMultithread(bool flag){
        isMultithread=flag;
    }
    bool start();
};

#endif
