#include "Server.h"

Server::Server(quint16 p, QObject* parent):QTcpServer(parent), port(p)
{
    //Create a Thread Pool
    pool = new QThreadPool(this);
    //writeLog("Num of threads in pool: " + QString::number(QThread::idealThreadCount()));
}

void Server::start()
{
    if (!this->listen(QHostAddress::Any, port)) {
        writeLog("Unable to start the server: " + this->errorString(), QtCriticalMsg);
        return;
    }
    writeLog("Server started.");
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    // 1. QTcpServer gets a new connection request from a client.
    // 2. It makes a task (runnable) here.
    // 3. Then, the server grabs one of the threads.
    // 4. The server throws the runnable to the thread.


    // Every new connection will be run in a newly created thread
    ClientHandler *ch = new ClientHandler(socketDescriptor, this);

    // IF ch WAS A QTHREAD
    // connect signal/slot
    // once a thread is not needed, it will be beleted later
    // connect(ch, &ClientHandler::finished, ch, &ClientHandler::deleteLater);

    ch->setMultithread(this->isMultithread);
    ch->handle();
}
