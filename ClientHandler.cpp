#include "ClientHandler.h"

ClientHandler::ClientHandler(qintptr ID, QObject *parent) :
    QObject(parent)
{
    this->socketDescriptor = ID;
}

void ClientHandler::handle()
{
    socket = new QTcpSocket();

    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        emit error(socket->error());
        return;
    }

    // connect socket and signal
    // note - Qt::DirectConnection is used because it's multithreaded
    //        This makes the slot to be invoked immediately, when the signal is emitted.
    connect(socket, SIGNAL(readyRead()),
            this, SLOT(readyRead()),
            Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()),
            this, SLOT(disconnected()));

    // alternative with lambda
    //    connect(socket, &QTcpSocket::readyRead,
    //            [&](){//code_for_reading});

    writeLog("Client connected: " + QString::number(socketDescriptor));
}

void ClientHandler::readyRead()
{
    Handle *handle = new Handle(socketDescriptor);
    handle->setAutoDelete(true);

    // get the information
    QByteArray data = socket->readAll();

    // will write on server side window
    writeLog(QString::number(socket->socketDescriptor()) + " Data in: " + data);


    QJsonDocument jDoc = QJsonDocument::fromBinaryData(data);
    QJsonObject jObj = jDoc.object();

    Record r;
    r.sender_mac = jObj["sender_mac"].toString();
    r.timestamp = jObj["sender_mac"].toInt();
    r.rssi = jObj["sender_mac"].toInt();
    r.hashed_pkt = jObj["sender_mac"].toString();
    r.ssid = jObj["sender_mac"].toString();

    deviceFinder->pushRecord(r);

    socket->write("OK\r\n");

    socket->flush();
    socket->waitForBytesWritten();
    socket->close();

//    if(isMultithread){
//        QThreadPool::globalInstance()->start(handle);
//    }else{
//        handle->run();
//    }
}

void ClientHandler::disconnected()
{
    writeLog(QString::number(socketDescriptor) + " Disconnected.");

    socket->deleteLater();

    // if it was a QThread
    //exit(0);
}
