
#include "serial_low_level.h"

const int serial_low_level::ringbuf_size = 255;

serial_low_level::serial_low_level(QObject *parent, QSettings *settings)
    : QObject(parent)
{    
    QStringList serial_ports = get_serial_ports();
    QString portName = settings->value("serial/port").toString();
    QString baud     = settings->value("serial/baud").toString();
    qint32 bautrate = toBaud(baud);

    qDebug() << "Opening serial port" << portName << "at" << bautrate << "baud.";

    if (serial_ports.contains(portName)) {
        serialPort.setPortName(portName);
        if (!serialPort.open(QIODevice::ReadWrite))
            qDebug() << QObject::tr("Failed to open port %1, error: %2").
                        arg(serialPort.portName()).
                        arg(serialPort.errorString()) << endl;

        serialPort.setBaudRate(bautrate);

        // asynchronuous data reception
        connect(&serialPort, SIGNAL(readyRead()), SLOT(handleReadyRead_slot()));

        receive_ringbuffer = new simpleRingbuffer(ringbuf_size);
    }
    else {
        qDebug() << "Serial port " << portName << " not available.";
    }
}

qint32 serial_low_level::toBaud(QString baudStr) {

    if (baudStr == "115200")
        return QSerialPort::Baud115200;
    else if (baudStr == "57600")
        return QSerialPort::Baud57600;
    else if (baudStr == "38400")
        return QSerialPort::Baud38400;
    else if (baudStr == "19200")
        return QSerialPort::Baud19200;
    else if (baudStr == "9600")
        return QSerialPort::Baud9600;
    else if (baudStr == "4800")
        return QSerialPort::Baud4800;
    else if (baudStr == "2400")
        return QSerialPort::Baud2400;

    // default
    return QSerialPort::Baud1200;
}



void serial_low_level::handleReadyRead_slot(void)
{
    QByteArray m_readData;
    m_readData.append(serialPort.readAll());

    //qDebug() << "handleReadyRead_slot: data received: " << m_readData.count();
    //QDebug deb = qDebug();

    foreach (const char rec_char, m_readData)
    {
        //deb << (uint8_t) rec_char;
        receive_ringbuffer->Write((uint8_t) rec_char);
    }
}


QStringList serial_low_level::get_serial_ports(void)
{
    QStringList  list;
    QString      tmp_str;
    QSerialPort  serial;

    // get names from every serial port and add it to the list
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        tmp_str.clear();

        // if the port could be opened, we add it to QStringList (and later to the combobox)
        serial.setPort(info);

        if (serial.isOpen())
            serial.close();

        if (serial.open(QIODevice::ReadWrite))
        {
            //qDebug() << "works: " << info.portName();
            tmp_str.append("/dev/");
            tmp_str.append(info.portName());
            list.append(tmp_str);
            serial.close();
        }
    }
    return list;
}


/*void serial_low_level::call_serial_setup_dialog(QWidget *parent)
{
    SettingsDialog dialog(&serialPort, parent);
    dialog.exec();
    SettingsDialog::Settings serial_settings = dialog.settings();

    if (serialPort.isOpen())
        serialPort.close();

    serialPort.setPortName(serial_settings.name);
    if (!serialPort.open(QIODevice::ReadWrite))
        qDebug() << QObject::tr("Failed to open port %1, error: %2").arg(serial_settings.name).arg(serialPort.errorString()) << endl;

    serialPort.setBaudRate(serial_settings.baudRate);
    serialPort.setDataBits(serial_settings.dataBits);
    serialPort.setParity(serial_settings.parity);
    serialPort.setStopBits(serial_settings.stopBits);
    serialPort.setFlowControl(serial_settings.flowControl);
}*/


bool serial_low_level::send_byte(uint8_t byte)
{
    if (serialPort.isOpen())
        serialPort.write((char*) &byte, 1);

    return true;
}


bool serial_low_level::send_many_bytes(uint8_t *ptr_to_data, uint8_t len)
{
    if (serialPort.isOpen())
        serialPort.write((char*) ptr_to_data, len);

    return true;
}


bool serial_low_level::is_TX_pending(void)
{
    return false;
}


simpleRingbuffer* serial_low_level::rx_ringbuffer(void)
{
    return receive_ringbuffer;
}

void serial_low_level::close_port(void)
{
    serialPort.close();
}













