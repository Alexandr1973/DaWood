#ifndef CRC32_H
#define CRC32_H

#include <QObject>

class crc32
{
public:
    crc32();
    unsigned int CRC32 (uint8_t* data, unsigned int length);
};

#endif // CRC32_H
