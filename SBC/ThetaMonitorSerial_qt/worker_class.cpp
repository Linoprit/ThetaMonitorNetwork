#include "worker_class.h"
#include <qcoreapplication.h>
#include <qstring.h>
#include <qfile.h>
#include <QStandardPaths>
#include <QProcessEnvironment>



worker_class::worker_class(QObject *parent) :
    QObject(parent)
{
    loop_timer = new QTimer(this);
    loop_timer->start(20);
    connect (this->loop_timer, SIGNAL(timeout()), this, SLOT(loop()));
    selectIniFile();

    serial          = new serial_low_level(this, settings);
    comm            = new Comm_Layer(serial);
    dbcon           = new DBconnection(settings);
}

void worker_class::selectIniFile(void) {
    QString home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString filename = home + "/thetaMonitor.ini";

    if (!QFile::exists(filename))
        filename = "/etc/thetaMonitor.ini";

    if (QFile::exists(filename)) {
        settings = new QSettings(filename, QSettings::IniFormat);
    }
    else
    {
        qDebug() << "No ini-file found, writing defaults to home-dir.";
        filename = home + "/thetaMonitor.ini";
        QFile file(filename);
        if (file.open(QIODevice::ReadWrite)) {
            QTextStream stream(&file);
            stream << "[database]" << endl;
            stream << "hostname=192.168.0.105" << endl;
            stream << "databasename=ThetaSensorData" << endl;
            stream << "username=StmClient" << endl;
            stream << "password=passwd" << endl;
            stream << "sensor_ID_table=Sensor_ID_Table" << endl;
            stream << "data_table=TestSensordata" << endl;
            stream << "" << endl;
            stream << "[serial]" << endl;
            stream << "port=/dev/ttyUSB0" << endl;
            stream << "baud=115200" << endl;
            stream << "" << endl;
        }
        file.close();

        exit(0);
    }
}

void worker_class::loop(void)
{
    comm->loop();
    QQueue<Messages_Base::nRF_theta_struct>* thetaQueue =
            Messages_Base::getThetaQueue();

    while (!thetaQueue->isEmpty())
    {
        dbcon->writeSensorValue(thetaQueue->dequeue());
    }



    /*QStringList result = dbcon->getLocation("28:FF:03:B7:C2:17:01:D1");
    foreach (QString tmpStr, result)
    {
        qDebug() << tmpStr;
    }*/


}

