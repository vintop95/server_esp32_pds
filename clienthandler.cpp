/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#include "clienthandler.h"
#include <iostream>


/**
 * @brief Constructor
 *
 * @param socket descriptor of the new client
 */
ClientHandler::ClientHandler(qintptr s, QObject *parent) :
    QObject(parent), deviceFinder(DeviceFinder::getInstance())
{
    setSocketDescriptor(s);
}


void ClientHandler::setSocketDescriptor(qintptr id)
{
    this->socketDescriptor = id;
}

void ClientHandler::handle()
{
    writeLog("#ClientHandler");
    // after waitPeriod close the connection
    connect(&timeoutDisconnect, &QTimer::timeout,
    [=](){
        if (socket->isValid())
            socket->disconnectFromHost();
        writeLog("Timeout expired.", QtWarningMsg);
    });
    timeoutDisconnect.setInterval(timeoutPeriod);
    timeoutDisconnect.start();

    socket = new QTcpSocket(this);

    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        writeLog("Cannot set socket " + QString::number(socketDescriptor), QtCriticalMsg);
        emit error(socket->error());
        return;
    }

    // Assign the callback to call when data is received from the client
    // (and when it disconnects)
    connect(socket, &QTcpSocket::readyRead,
            this, &ClientHandler::readFromSocket);
    connect(socket, &QTcpSocket::disconnected,
            this, &ClientHandler::closeClientHandler);

    // alternative connect syntax with lambda
    //    connect(socket, &QTcpSocket::readyRead,
    //            [&](){//code_for_reading});

    writeLog("Client connected: " + QString::number(socketDescriptor));
}


/**
 * @brief Callback called when the server received data from client
 * This function works together with 'Sender::sendPacketsToServer()' function
 * in the client
 *
 * @todo: add catch for runtime exceptions (like the socket already closed)
 */
void ClientHandler::readFromSocket()
{
    writeLog("#ClientHandler");
    // get the information
    // https://stackoverflow.com/questions/28955961/qt-socket-does-no-read-all-data
    // è possibile ricevere i dati in modo incompleto, quindi bisogna mantenere uno stato
    // per essere sicuro di aver ricevuto tutti i dati


    if(!dataReceived.isEmpty()){
        // There is a connection to conclude
        writeLog("INITIAL STATE OF DATA VARIABLE: " + dataReceived);
        dataReceived += socket->readAll();
        writeLog(QString::number(socket->socketDescriptor()) + " - MSG RCVD: " + dataReceived);
        this->pushPacketsToDeviceFinder();//save the packets
        return;
    }

    dataReceived = socket->peek(10);

    // AGGIUNGERE IF INIT, DATA, END (chiudi)
    if(dataReceived.startsWith("INIT ")){
        QByteArray msg;
        socket->skip(strlen("INIT "));
        msg = socket->readAll();
        writeLog(QString::number(socket->socketDescriptor()) + " - MSG RCVD: " + msg);
        msg = msg.replace('\0', '\n');

        writeLog(QString::number(socketDescriptor) + " - INIT MESSAGE");
        // Deserialize data received
        QJsonDocument jDoc = QJsonDocument::fromJson(msg);

        if(!jDoc.isNull() && jDoc.isObject()){
            QJsonObject jObj = jDoc.object();
            espName = jObj["name"].toString();
            writeLog(QString::number(socketDescriptor) + " - NAME OF ESP IS " + espName, QtInfoMsg);

            // AUTHENTICATION
            if(Settings::getInstance()->esp32s->count(espName) != 0){
                writeLog(QString::number(socketDescriptor) + " - " + espName + " AUTHENTICATED", QtInfoMsg);
                socket->write("OK\r\n");
            }else{
                writeLog(QString::number(socketDescriptor) + " - " + espName + " NOT AUTHENTICATED", QtWarningMsg);
                socket->write("ERR\r\n");
            }

        }else{
            writeLog(QString::number(socketDescriptor) + " - ERROR IN GETTING ESPNAME", QtCriticalMsg);
            socket->write("ERR\r\n");
        }

        socket->flush();
        //socket->waitForBytesWritten();
        dataReceived.clear();
    }
    else if(dataReceived.startsWith("DATA ")){

        if(espName == "UNKNOWN"){
            writeLog(QString::number(socketDescriptor) + " - ESPNAME UNKNOWN", QtCriticalMsg);
            socket->write("ERR\r\n");
            socket->flush();
            //socket->waitForBytesWritten();
            dataReceived.clear();
            return;
        }

        writeLog(QString::number(socketDescriptor) + " - DATA MESSAGE");

        socket->skip(strlen("DATA "));

        dataReceived = socket->readAll();
        this->pushPacketsToDeviceFinder();

    }
    else if(dataReceived.startsWith("END")){
        writeLog(QString::number(socketDescriptor) + " - END MESSAGE");

        // Acknowledge the client
        // And send him the timestamp
        uint timestamp_now = QDateTime::currentDateTime().toTime_t();

        //QString msgOut = "OK " + QString::number(tmpstmp) + "\r\n";
        QString msgOut = "OK ";
        socket->write(msgOut.toStdString().c_str(), msgOut.length());
        timestamp_now = qToBigEndian(timestamp_now);
        socket->write((const char*) &timestamp_now, sizeof(timestamp_now));
        msgOut = "\r\n";
        socket->write(msgOut.toStdString().c_str(), msgOut.length());
        socket->flush();
        //socket->waitForBytesWritten();

        dataReceived.clear();

        // Close the connection emitting the disconnected signal
        if (socket->isValid())
            socket->disconnectFromHost();

    }else{
        writeLog(QString::number(socketDescriptor) + " - FORMAT NOT RECOGNIZED");
        dataReceived = socket->readAll();
        writeLog(QString::number(socket->socketDescriptor()) + " - MSG RCVD: " + dataReceived);
        socket->write("ERR\r\n");
        socket->flush();
        //socket->waitForBytesWritten();
        dataReceived.clear();
    }


}

/**
 * @brief Callback called when the client disconnects
 */
void ClientHandler::closeClientHandler()
{
    writeLog("#ClientHandler");
    writeLog(QString::number(socketDescriptor) + " Disconnected.");

    timeoutDisconnect.stop();

    if (socket->isValid())
        socket->disconnectFromHost();

    socket->deleteLater();

    this->deleteLater();

    // if it was a QThread
    //exit(0);
}

void ClientHandler::pushPacketsToDeviceFinder()
{
    writeLog("#ClientHandler");
    dataReceived = dataReceived.replace('\0', '\n');

    // Deserialize data received
    QJsonDocument jDoc = QJsonDocument::fromJson(dataReceived);
    if(!jDoc.isNull() && jDoc.isArray()){
        // IF WE RECEIVED ALL THE JSON ARRAY
        writeLog(QString::number(socketDescriptor) + " - PACKETS RECEIVED", QtInfoMsg);

        QJsonArray packets = jDoc.array();

        for(auto pktRcvd : packets){
            QJsonObject obj = pktRcvd.toObject();
            Packet r;
            r.sender_mac = obj["sender_mac"].toString();
            r.timestamp = static_cast<quint32>(obj["timestamp"].toInt());
            r.rssi = static_cast<qint8>(obj["rssi"].toInt());
            r.hashed_pkt = obj["hashed_pkt"].toString();
            r.ssid = obj["ssid"].toString();
            r.espName = this->espName;
            deviceFinder->pushPacket(r);
        }
        //TODO: aggiungere metodo di device finder che aggiunga i packets accumulati nel database
        deviceFinder->insertPacketsIntoDB(espName);
        socket->write("OK\r\n");
        dataReceived.clear();

    }else{
        writeLog(QString::number(socketDescriptor) + " - ERROR GETTING PACKET, it may arrive later the rest", QtWarningMsg);
        //socket->write("ERR\r\n");
    }
    socket->flush();
    //socket->waitForBytesWritten();
}
