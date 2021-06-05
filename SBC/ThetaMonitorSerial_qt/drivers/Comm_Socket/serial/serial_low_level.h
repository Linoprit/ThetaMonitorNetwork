#ifndef SERIAL_LOW_LEVEL_H
#define SERIAL_LOW_LEVEL_H


#include <QtSerialPort/QSerialPort>
#include <QSerialPortInfo>
#include <QObject>
#include <QString>
#include <QByteArray>
#include <qdebug.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <QSettings>
#include "../Comm_Socket.h"
//#include "settingsdialog.h"
#include "../../../Libraries/simpleRingbuffer.h"


QT_USE_NAMESPACE

class serial_low_level: public QObject, public Comm_Socket
{
    Q_OBJECT

public:
    QSerialPort serialPort;

    serial_low_level(QObject *parent = 0, QSettings* settings = 0);
    static  QStringList get_serial_ports(void);
    void    call_serial_setup_dialog(QWidget *parent = 0);

    bool send_byte(uint8_t byte);
    bool send_many_bytes(uint8_t *ptr_to_data, uint8_t len);
    bool is_TX_pending(void);
    simpleRingbuffer* rx_ringbuffer(void);
    void close_port(void);
    qint32 toBaud(QString baudStr);

private slots:
    void handleReadyRead_slot(void);


private:    
    simpleRingbuffer    *receive_ringbuffer;
    static const int    ringbuf_size;
};

#endif // SERIAL_LOW_LEVEL_H
