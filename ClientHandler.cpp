/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#include "ClientHandler.h"

/**
 * @brief Constructor
 *
 * @param socket descriptor of the new client
 */
ClientHandler::ClientHandler(qintptr ID, QObject *parent) :
    QObject(parent)
{
    this->socketDescriptor = ID;
}

/**
 * @brief Handles the client
 */
void ClientHandler::handle()
{
    socket = new QTcpSocket();

    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        writeLog("Cannot set socket " + QString::number(socketDescriptor), QtCriticalMsg);
        emit error(socket->error());
        return;
    }

    // Assign the callback to call when data is received from the client
    // (and when it disconnects)
    // note - Qt::DirectConnection is used because it's multithreaded
    //        This makes the slot to be invoked immediately, when the signal is emitted.
    connect(socket, SIGNAL(readyRead()),
            this, SLOT(readyRead()),
            Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()),
            this, SLOT(disconnected()));

    // alternative connect syntax with lambda
    //    connect(socket, &QTcpSocket::readyRead,
    //            [&](){//code_for_reading});

    writeLog("Client connected: " + QString::number(socketDescriptor));
}

/**
 * @brief Callback called when the server received data from client
 */
void ClientHandler::readyRead()
{
    // get the information
    // TODO: enhance
    QByteArray data = socket->readAll();

    // will write on server side window
    writeLog(QString::number(socket->socketDescriptor()) + " Data in: " + data);

    // Deserialize data received
    QJsonDocument jDoc = QJsonDocument::fromBinaryData(data);
    QJsonObject jObj = jDoc.object();
    Record r;
    r.sender_mac = jObj["sender_mac"].toString();
    r.timestamp = jObj["sender_mac"].toInt();
    r.rssi = jObj["sender_mac"].toInt();
    r.hashed_pkt = jObj["sender_mac"].toString();
    r.ssid = jObj["sender_mac"].toString();

    // Push the record received
    deviceFinder->pushRecord(r);

    // Acknowledge the client
    socket->write("OK\r\n");
    socket->flush();
    socket->waitForBytesWritten();

    // Close the connection
    socket->close();

    // Decomment to use multithreading
//    Handle *handle = new Handle(socketDescriptor);
//    handle->setAutoDelete(true);
//    if(isMultithread){
//        QThreadPool::globalInstance()->start(handle);
//    }else{
//        handle->run();
//    }
}

/**
 * @brief Callback called when the client disconnects
 */
void ClientHandler::disconnected()
{
    writeLog(QString::number(socketDescriptor) + " Disconnected.");

    socket->deleteLater();

    // if it was a QThread
    //exit(0);
}
