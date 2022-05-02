#ifndef PIEZO_H
#define PIEZO_H

#include <QObject>
#include <QDateTime>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <QDebug>
#include <QTemporaryDir>

class piezo : public QObject
{
    Q_OBJECT
public:
    piezo();
    int packageProcessing(QString pathdatalogs, uint8_t* bufferData, uint32_t length);

signals:
        void sendCurrentNoFProcess(int);
};

#endif // PIEZO_H
