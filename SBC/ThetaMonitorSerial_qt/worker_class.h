#ifndef MAIN_LOOP_H
#define MAIN_LOOP_H

#include <QObject>
#include <QTimer>
#include <QDebug>
#include <QSettings>
#include "drivers/Comm_Socket/serial/serial_low_level.h"
#include "system/Comm_Layer/Comm_Layer.h"
#include "database/dbConnection.h"


class worker_class : public QObject
{
    Q_OBJECT
public:


    explicit worker_class(QObject *parent = 0);
    serial_low_level    *serial;


signals:

public slots:
    void loop(void);

private:
    QTimer*         loop_timer;
    //Messages_Base*  messages_base;
    Comm_Layer*     comm;
    QSettings*      settings;
    DBconnection*   dbcon;

    void selectIniFile(void);
};

#endif // MAIN_LOOP_H
