#-------------------------------------------------
#
# Project created by QtCreator 2018-05-11T18:17:57
#
#-------------------------------------------------

QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = S3C2440_Controller
TEMPLATE = app


SOURCES += main.cpp\
        controlwidget.cpp \
    myserialport.cpp \
    portclient.cpp

HEADERS  += controlwidget.h \
    myserialport.h \
    portclient.h \
    MLTP.h

FORMS    += controlwidget.ui

RESOURCES += \
    qm.qrc
