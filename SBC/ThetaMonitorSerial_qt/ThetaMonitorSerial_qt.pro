#-------------------------------------------------
#
# Project created by QtCreator 2019-02-23T21:00:04
#
#-------------------------------------------------

#QT       += c++11 
#QT       += console
QT       += core
QT	 -= gui
QT       += serialport
QT       += sql

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ThetaMonitorSerial_qt
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


SOURCES += \
        main.cpp \
        drivers/Comm_Socket/Comm_Socket.cpp \
        drivers/Comm_Socket/serial/serial_low_level.cpp \
        Libraries/simpleRingbuffer.cpp \
        system/Comm_Layer/Comm_Layer.cpp \
        system/Comm_Layer/Messages_Base.cpp \
        worker_class.cpp \
        database/dbConnection.cpp

HEADERS += \
        drivers/Comm_Socket/Comm_Socket.h \
        drivers/Comm_Socket/serial/serial_low_level.h \
        Libraries/simpleRingbuffer.h \
        system/Comm_Layer/Comm_Layer.h \
        system/Comm_Layer/Messages_Base.h \
        worker_class.h \
        database/dbConnection.h

#FORMS += \
#        mainwindow.ui \
#        drivers/Comm_Socket/serial/settingsdialog.ui

QMAKE_CXXFLAGS += -DQT_TARGET
