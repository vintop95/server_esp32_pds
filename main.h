#ifndef MAIN_H
#define MAIN_H

#include <QObject>
#include <QDebug>
#include <QApplication>
#include <QMainWindow>
#include <QMessageBox>
#include <QWidget>
#include <QDialog>
#include <QString>
#include <QVector>
#include <QtWidgets>
#include <QtNetwork>
#include <QtCore>
#include <QTime>

#include "mainwindow.h"
#include "logger.h"

using namespace std;


#define BUFSIZE 1024
#define SERVER_PORT 7856

static MainWindow* pWin;

void writeLog(const QString &text, QtMsgType type = QtDebugMsg);

static Logger* pLog;

#endif // MAIN_H
