#-------------------------------------------------
#
# Project created by QtCreator 2018-09-17T17:34:22
#
#-------------------------------------------------


QT += core gui network widgets
QT += charts sql printsupport

TARGET = server_esp32_pds
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    logger.cpp \
    dbmanager.cpp \
    record.cpp \
    settings.cpp \
    settingsform.cpp \
    espwidget.cpp \
    areachart.cpp \
    devicefinder.cpp \
    server.cpp \
    clienthandler.cpp \
    chart.cpp \
    qcustomplot.cpp

HEADERS += \
        main.h \
        mainwindow.h \
    logger.h \
    dbmanager.h \
    record.h \
    settings.h \
    settingsform.h \
    espwidget.h \
    areachart.h \
    devicefinder.h \
    server.h \
    clienthandler.h \
    chart.h \
    qcustomplot.h

FORMS += \
        mainwindow.ui \
    settingsform.ui \
    espwidget.ui

RC_FILE = server_esp32_pds.rc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    classmodel.qmodel

STATECHARTS +=
