#include "myserialport.h"

#include <QMessageBox>
#include <QObject>

MySerialPort::MySerialPort()
{

    //openSerialPort();
}

QList<QSerialPortInfo> MySerialPort::getSerialPorts()
{
    QList<QSerialPortInfo> com_ports = QSerialPortInfo::availablePorts();
    return com_ports;
}

void MySerialPort::CreateConnect()
{
    serial = new QSerialPort(this);
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData2()));
    connect(serial, SIGNAL(bytesWritten(qint64)), this, SLOT(writeComplete(qint64)));
    qDebug() << "Signal connected";
}

int MySerialPort::openSerialPort(const QString port)
{
    int result = 0;
    serial->setPortName(port);
    serial->setBaudRate(921600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    if (serial->open(QIODevice::ReadWrite))
    {
        showStatusMessage("Connected");
        result = 1;

    }
    else
    {
        showStatusMessage("Open error");
        result = -1;
    }
    return result;
}

void MySerialPort::closeSerialPort(void)
{
    if(serial->isOpen())
    {
        showStatusMessage("Close");
        serial->close();
    }
}

void MySerialPort::readData2()
{
    this->data = serial->readAll();
//    qDebug() << "Data lenght: " << this->data.length() << "Data: " << this->data.toHex();
}


void MySerialPort::showStatusMessage(const QString &message)
{
    qDebug() << message;
}

QByteArray MySerialPort::getData()
{
    return this->data;
}

void MySerialPort::clearData()
{
    this->data.clear();
}

void MySerialPort::writeComplete(qint64 t)
{
//    qDebug() << "Hello";
}
void MySerialPort::sendData(char* data, uint32_t length)
{
    if(serial->isOpen())
    {
        this->serial->write(data, length);
        while(this->serial->waitForBytesWritten() == false);    //Ждем пока не отправится
    }
}

