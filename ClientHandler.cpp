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
    QThread(parent), deviceFinder(dF)
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

void ClientHandler::run()
{

}

/**
 * @brief Callback called when the server received data from client
 * This function works together with 'Sender::sendRecordsToServer()' function
 * in the client
 */
void ClientHandler::readyRead()
{
    int msWaitingTime = 3000;
    int bufferSize = 1024;
    // get the information
    // https://stackoverflow.com/questions/28955961/qt-socket-does-no-read-all-data
    // è possibile ricevere i dati in modo incompleto, quindi bisogna mantenere uno stato
    // per essere sicuro di aver ricevuto tutti i dati

    QByteArray data = socket->peek(10);


    // AGGIUNGERE IF INIT, DATA, END (chiudi)
    if(data.startsWith("INIT ")){
        socket->skip(strlen("INIT "));
        data = socket->readAll();
        writeLog(QString::number(socket->socketDescriptor()) + " - MSG RCVD: " + data);
        data = data.replace('\0', '\n');

        writeLog(QString::number(socketDescriptor) + " - INIT MESSAGE");
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

        if(espName == "UNKNOWN"){
            writeLog(QString::number(socketDescriptor) + " - ESPNAME UNKNOWN", QtCriticalMsg);
            socket->write("ERR\r\n");
            socket->flush();
            socket->waitForBytesWritten();
            return;
        }

        socket->skip(strlen("DATA "));
        int bytesRead;


        data = "";
        QJsonDocument jDoc;
        do{
            bytesRead = bufferSize;
            data += socket->read(bytesRead);

            data = data.replace('\0', '\n');

            // Deserialize data received
            jDoc = QJsonDocument::fromJson(data);
            if(!jDoc.isNull() && jDoc.isArray()){
                // IF WE RECEIVED ALL THE JSON ARRAY
                break;
            }
            // da fare in un thread ulteriore perché bloccante
            if (!socket->waitForReadyRead(msWaitingTime)){
                writeLog("waitForReadyRead() timed out", QtCriticalMsg);
                socket->write("ERR\r\n");
                socket->flush();
                socket->waitForBytesWritten();
                return;
            }

        }while(bytesRead > 0);

        writeLog(QString::number(socketDescriptor) + " - DATA MESSAGE");

        if(!jDoc.isNull() && jDoc.isArray()){
            writeLog(QString::number(socketDescriptor) + " - RECORD RECEIVED", QtInfoMsg);

            QJsonArray records = jDoc.array();

            for(auto recRcvd : records){
                QJsonObject obj = recRcvd.toObject();
                Record r;
                r.sender_mac = obj["sender_mac"].toString();
                r.timestamp = obj["timestamp"].toInt();
                r.rssi = obj["rssi"].toInt();
                r.hashed_pkt = obj["hashed_pkt"].toString();
                r.ssid = obj["ssid"].toString();
                r.espName = this->espName;
                deviceFinder->pushRecord(r);
            }

            socket->write("OK\r\n");
        }else{
            writeLog(QString::number(socketDescriptor) + " - ERROR GETTING RECORD", QtCriticalMsg);
            socket->write("ERR\r\n");
        }
        socket->flush();
        socket->waitForBytesWritten();
    }
    else if(data.startsWith("END")){
        writeLog(QString::number(socketDescriptor) + " - END MESSAGE");
        socket->close();

        // Acknowledge the client
        socket->write("OK\r\n");
        socket->flush();
        socket->waitForBytesWritten();
    }else{
        writeLog(QString::number(socketDescriptor) + " - FORMAT NOT RECOGNIZED");
        data = socket->readAll();
        writeLog(QString::number(socket->socketDescriptor()) + " - MSG RCVD: " + data);
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
