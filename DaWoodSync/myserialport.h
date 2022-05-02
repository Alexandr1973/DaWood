#ifndef MYSERIALPORT_H
#define MYSERIALPORT_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QDebug>
#include <QThread>
#include <QtSerialPort/QSerialPortInfo>
#include <QList>

class MySerialPort: public QSerialPort
{
    Q_OBJECT
public:
    MySerialPort();

    QByteArray getData();
    void clearData();
    int openSerialPort(const QString port);
    void closeSerialPort(void);
    QList<QSerialPortInfo> getSerialPorts(void);
    void CreateConnect(void);
public slots:
    void sendData(char*, uint32_t);
    void readData2();
    void writeComplete(qint64);
private:
    void showStatusMessage(const QString &message);
    QByteArray data;
    QSerialPort *serial;
};

#endif // MYSERIALPORT_H
