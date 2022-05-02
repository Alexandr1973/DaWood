#ifndef FIRMWARE_READ_H
#define FIRMWARE_READ_H

#include <QObject>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <QDebug>
#include <QTemporaryDir>

class firmware_read
{
public:
    firmware_read();
    int getLength (QString pathfirmware);
    int getFirmware (QString pathfirmware, uint8_t *firmware);
};

#endif // FIRMWARE_READ_H
