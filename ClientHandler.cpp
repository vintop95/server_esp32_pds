/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#include "ClientHandler.h"
#include <iostream>

/**
 * @brief Constructor
 *
 * @param socket descriptor of the new client
 */
ClientHandler::ClientHandler(qintptr ID, DeviceFinder* dF, QObject *parent) :
    QObject(parent), deviceFinder(dF)
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
 * This function works together with 'SendRecordsToServer()' function
 * in the client
 */
void ClientHandler::readyRead()
{
    // get the information
    // TODO: enhance
    // https://stackoverflow.com/questions/7923485/simplest-qt-tcp-client
    // è possibile ricevere i dati in modo incompleto, quindi bisogna mantenere uno stato
    // per essere sicuro di aver ricevuto tutti i dati

    /*
     * // Loop reading data.
        while(1) {
            ssize_t sizeRead = recv(clientSock, data + sizeUsed, total-sizeUsed, 0);
            if (sizeRead < 0) {
                ESP_LOGE(tag, "recv: %d %s", sizeRead, strerror(errno));
                goto END;
            }
            if (sizeRead == 0) {
                break;
            }
            sizeUsed += sizeRead;
        }
    */

    /*
        do {
            bzero(recv_buf, sizeof(recv_buf));
            r = read(s, recv_buf, sizeof(recv_buf)-1);
            for(int i = 0; i < r; i++) {
                putchar(recv_buf[i]);
            }
        } while(r > 0);
    */

    QByteArray data = socket->readAll();
    data = data.replace('\0', '\n');

    // will write on server side window
    writeLog(QString::number(socket->socketDescriptor()) + " - Data in: " + data);


    // AGGIUNGERE IF INIT, DATA, END (chiudi)
    if(data.startsWith("INIT ")){
        writeLog(QString::number(socketDescriptor) + " - INIT MESSAGE");
        data = data.remove(0,strlen("INIT "));
        // Deserialize data received
        QJsonDocument jDoc = QJsonDocument::fromJson(data);

        if(!jDoc.isNull() && jDoc.isObject()){
            QJsonObject jObj = jDoc.object();
            espName = jObj["name"].toString();
            writeLog(QString::number(socketDescriptor) + " - NAME OF ESP IS " + espName, QtInfoMsg);
            socket->write("OK\r\n");
        }else{
            writeLog(QString::number(socketDescriptor) + " - ERROR IN GETTING ESPNAME", QtCriticalMsg);
            socket->write("ERR\r\n");
        }

        socket->flush();
        socket->waitForBytesWritten();
    }
    else if(data.startsWith("DATA ")){
        writeLog(QString::number(socketDescriptor) + " - DATA MESSAGE");

        if(espName == "UNKNOWN"){
            writeLog(QString::number(socketDescriptor) + " - ESPNAME UNKNOWN", QtCriticalMsg);
            return;
        }

        data = data.remove(0,strlen("DATA "));
        // Deserialize data received
        QJsonDocument jDoc = QJsonDocument::fromJson(data);

        Record r;
        if(!jDoc.isNull() && jDoc.isObject()){
            QJsonObject jObj = jDoc.object();
            r.sender_mac = jObj["sender_mac"].toString();
            r.timestamp = jObj["timestamp"].toInt();
            r.rssi = jObj["rssi"].toInt();
            r.hashed_pkt = jObj["hashed_pkt"].toString();
            r.ssid = jObj["ssid"].toString();
            r.espName = this->espName;
            writeLog(QString::number(socketDescriptor) + " - RECORD RECEIVED", QtInfoMsg);

            // Push the record received
            deviceFinder->pushRecord(r);
        }else{
            writeLog(QString::number(socketDescriptor) + " - ERROR GETTING RECORD", QtCriticalMsg);
            socket->write("ERR\r\n");
            socket->flush();
            socket->waitForBytesWritten();
        }

    }
    else if(data.startsWith("END")){
        writeLog(QString::number(socketDescriptor) + " - END MESSAGE");
        socket->close();

        // Acknowledge the client
        socket->write("OK\r\n");
        socket->flush();
        socket->waitForBytesWritten();
    }else{
        socket->write("ERR\r\n");
        socket->flush();
        socket->waitForBytesWritten();
    }
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
