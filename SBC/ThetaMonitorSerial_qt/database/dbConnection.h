#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QSettings>
#include "system/Comm_Layer/Messages_Base.h"


class DBconnection
{
public:
    DBconnection(QSettings* settings);

    bool writeSensorValue(
            Messages_Base::nRF_theta_struct thetaStruct);
    QStringList getLocation(QString ID_text);

private:
    QString build_ID_text(uint8_t* ID);
    QString sensor_ID_table;
    QString data_table;
};

#endif // DBCONNECTION_H
