#ifndef UTILITES_H
#define UTILITES_H

#include <QObject>
#include <QDebug>
#include <QDateTime>
class utilites
{
public:
    utilites();
    static void Convert32to8bits (uint8_t *Packet8, uint32_t ValueNeedPack);
    static void Convert16to8bits (uint8_t *Packet8, uint16_t ValueNeedPack);
    static uint32_t Convert8to32bits (uint8_t* Packet8);
    static uint16_t Convert8to16bits (uint8_t* Packet8);
    static void ConvertedFloatTo8_32(uint8_t *Packet8, float fraw);
    static uint32_t ConvertedFloatTo32(float fraw);
    static float Converted8_32ToFloat(uint8_t *Packet8);
    static float Converted32ToFloat(uint32_t raw);
    static uint32_t GetLocalTime(void);
};

#endif // UTILITES_H
