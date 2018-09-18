#ifndef CLIENT_H
#define CLIENT_H

#include "main.h"
#include "ClientHandler.h"
#include <QThreadPool>

class Server : public QTcpServer
{
    Q_OBJECT
private:
    quint16 port;
    QTcpSocket *socket;
    bool isMultithread = true;
    QThreadPool *pool;
signals:

public slots:

protected:
    void incomingConnection(qintptr socketDescriptor) override;

public:
    Server(quint16 port, QObject* parent = nullptr);
    void setMultithread(bool flag){
        isMultithread=flag;
    }
    void start();
};

#endif // CLIENT_H
