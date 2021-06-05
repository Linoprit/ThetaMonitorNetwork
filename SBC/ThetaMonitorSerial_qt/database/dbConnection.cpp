#include "dbConnection.h"
#include <QtSql/QSqlDatabase>
#include <qsqlquery.h>
#include <QSqlError>
#include "system/Comm_Layer/Messages_Base.h"

#include <qdebug.h>



DBconnection::DBconnection(QSettings* settings)
{
    //Q_UNUSED(settings);
    QString hostname = settings->value("database/hostname").toString();
    QString DBName   = settings->value("database/databasename").toString();
    QString userName = settings->value("database/username").toString();

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(hostname);
    db.setDatabaseName(DBName);
    db.setUserName(userName);
    db.setPassword(settings->value("database/password").toString());
    bool result = db.open();

    if (result == false)
        qDebug() << db.lastError();
    else {
        qDebug() << "DBopen: ok.";
        qDebug() << "DB is" << DBName << "with user" << userName <<
                    "at host" << hostname;
    }

    sensor_ID_table = settings->value("database/sensor_ID_table").toString();
    data_table      = settings->value("database/data_table").toString();
}

QStringList DBconnection::getLocation(QString ID_text)
{
    QStringList  list;
    QSqlQuery    query;
    QString      command =
            "SELECT Location_ID, Location"
            " FROM " + sensor_ID_table +
            " WHERE ID_text = '" + ID_text + "';";
    bool result = query.exec(command);

    if (result == false)
    {
        qDebug() << "Error in executing SQL-statement.";
    }
    else if(query.next())
    {
        list.append(query.value(0).toString()); // Location_ID
        list.append(query.value(1).toString()); // Location
    }
    else
    {
        qDebug() << "Sensor ID not found.";
    }

    if (list.isEmpty())
    {
        list.append("UNKNO");
        list.append("unknown location");
    }

    return list;
}

QString DBconnection::build_ID_text(uint8_t *ID)
{
    QString result;

    for (int i=0; i < DS1820_ADDRESS_LEN; i++)
    {
        result.append(QString("%1").arg(ID[i], 2, 16, QLatin1Char('0'))
                      .toUpper());
        if (i < (DS1820_ADDRESS_LEN -1 ))
            result.append(":");
    }
    return result;
}

bool DBconnection::writeSensorValue(
        Messages_Base::nRF_theta_struct thetaStruct)
{
    QString ID_text = build_ID_text(&thetaStruct.sensor_id[0]);
    QStringList location = getLocation(ID_text);

    QSqlQuery query;
    QString command = "INSERT INTO " + data_table +
            " (ID_text, Temperature, Location_ID, Location) "
            "VALUES (:ID_text, :Temperature, :Location_ID, :Location);";

    //qDebug() << command;

    query.prepare(command);
    query.bindValue(":ID_text", ID_text);
    query.bindValue(":Temperature", QString("%1").arg(thetaStruct.theta));
    query.bindValue(":Location_ID", location.at(0));
    query.bindValue(":Location", location.at(1));
    bool result = query.exec();

    if (result == false)
    {
        qDebug() << "Error in putting values into DB.";
        return false;
    }

    return true;
}
