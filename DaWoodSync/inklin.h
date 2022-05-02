#ifndef INKLIN_H
#define INKLIN_H

#include <QDateTime>
#include <QObject>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <QDebug>
#include <QTemporaryDir>

class inklin : public QObject
{
public:

    typedef struct inklinDataType
    {
        uint8_t serialnum[8];
        int16_t axisX;
        int16_t axisY;
        int16_t axisZ;
    }inklinData;


    inklin();
    int PackageProcessing_Inklin(QString pathdatalogs, uint8_t* bufferData, uint32_t length);

signals:
        void sendCurrentNoFProcess(int);
};

#endif // INKLIN_H
