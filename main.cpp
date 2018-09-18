#include "main.h"

#include "Server.h"
#include "Receiver.h"

void writeLog(const QString &text, QtMsgType type){
    pLog->writeLog(text,type);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    pWin = &w;
    Logger logger(pWin);
    pLog = &logger;
    w.show();

    writeLog("+++ WELCOME TO THE ESP32 SERVER +++", QtInfoMsg);

    Server c(SERVER_PORT);
    c.setMultithread(false);
    c.start();

    return a.exec();
}
