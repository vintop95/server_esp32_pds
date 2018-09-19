/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#ifndef MAIN_H
#define MAIN_H

/**
 * @brief
 *
 * @param
 *
 * @return
 */

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
#include <QSettings>

#include "mainwindow.h"
#include "logger.h"

using namespace std;

static quint16 SERVER_PORT = 7856;
static int ESP32_NO = 4;
// The application shows the devices in area each CHART_PERIOD ms
static int CHART_PERIOD = 10000;

static MainWindow* pWin;
static Logger* pLog;

void writeLog(const QString &text, QtMsgType type = QtDebugMsg);

#endif // MAIN_H
