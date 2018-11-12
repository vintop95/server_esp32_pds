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
#include "clienthandler.h"
#include <QThreadPool>

/**
 * This class models the TCP Server that receives
 * packets from the ESP32 devices
 */
class Server : public QTcpServer
{
    Q_OBJECT
private:
    static Server* instance;
    quint16 port;
    QTcpSocket *socket;
    bool isMultithread = true;

    Server(QObject* parent = nullptr);
    void setPort(quint16 p);
protected:
    void incomingConnection(qintptr socketDescriptor) override;

public:
    static Server* getInstance(quint16 p);
    void setMultithread(bool flag){
        isMultithread=flag;
    }
    bool start();
};

#endif
