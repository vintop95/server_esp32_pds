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
#include <QSharedPointer>

#include "mainwindow.h"
#include "logger.h"
#include "settings.h"

using namespace std;

static MainWindow* pWin;

void writeLog(const QString &text, QtMsgType type = QtDebugMsg);

#endif // MAIN_H
