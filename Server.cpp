/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#include "Server.h"

/**
 * @brief Constructor of Server
 */
Server::Server(quint16 p, DeviceFinder* dF, QObject* parent)
    :QTcpServer(parent), port(p), deviceFinder(dF)
{}

/**
 * @brief Starts the Server
 *
 * @return true if all went well
 */
bool Server::start()
{
    writeLog("#Server");
    bool res = listen(QHostAddress::Any, port);
    if (!res) {
        writeLog("Unable to start the server: " + this->errorString(), QtCriticalMsg);
        return res;
    }
    writeLog("Server started.");
    return res;
}

/**
 * @brief Callback called when a new connection is requested
 * The ClientHandler object will handle the client
 * It is a virtual function derived from QThread
 *
 * @param socket descriptor of the client connected
 */
void Server::incomingConnection(qintptr socketDescriptor)
{
    ClientHandler *ch = new ClientHandler(socketDescriptor, deviceFinder, this);
    ch->setMultithread(this->isMultithread);
    ch->handle();

//    if(this->isMultithread){
//        // once a thread is not needed, it will be beleted later
//        connect(ch, &ClientHandler::finished, ch, &ClientHandler::deleteLater);
//        ch->start();
//    }else{

//    }

}
