#ifndef THERMO_H
#define THERMO_H

#include <QDateTime>
#include <QObject>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <QDebug>
#include <QTemporaryDir>
#include "utilites.h"
typedef struct
{
    unsigned int NumberDevice;
    unsigned int ow_mac[8]; //8 bytes Mac addres one wire device
    float temperature;
    float coef1;
    float coef2;
    int NumberInPassport;
}pckt_thermobraid;

class thermo : public QObject
{
    Q_OBJECT
signals:
        void sendCurrentNoFProcess(int);
public:
    thermo();
    int packageProcessing(QString pathdatalogs, QString pathpassport, uint8_t*, uint32_t);
    int packageProcessing(QString pathdatalogs, uint8_t*, uint32_t);
private:
    int CalculateCoefAndFindPosition(QString path, pckt_thermobraid *packet);
    void sorting (int *Array, int LengthArray);
};

#endif // THERMO_H
