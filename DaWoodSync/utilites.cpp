#include "utilites.h"

utilites::utilites()
{

}

void utilites::Convert32to8bits (uint8_t *Packet8, uint32_t ValueNeedPack)
{
    Packet8[0] = (ValueNeedPack & 0xFF000000) >> 24;
    Packet8[1] = (ValueNeedPack & 0xFF0000) >> 16;
    Packet8[2] = (ValueNeedPack & 0xFF00) >> 8;
    Packet8[3] = (ValueNeedPack & 0xFF);
}

void utilites::Convert16to8bits (uint8_t *Packet8, uint16_t ValueNeedPack)
{
    Packet8[0] = (ValueNeedPack & 0xFF00) >> 8;
    Packet8[1] = (ValueNeedPack & 0xFF);
}

uint32_t utilites::Convert8to32bits (uint8_t* Packet8)
{
    uint32_t len;
    len = (uint8_t)Packet8[0] << 24;
    len |= (uint8_t)Packet8[1] << 16;
    len |= (uint8_t)Packet8[2] << 8;
    len |= (uint8_t)Packet8[3];
    return len;
}

uint16_t utilites::Convert8to16bits (uint8_t* Packet8)
{
    uint16_t len;
    len = (uint8_t)Packet8[0] << 8;
    len |= (uint8_t)Packet8[1];
    return len;
}

void utilites::ConvertedFloatTo8_32(uint8_t *Packet8, float fraw)
{
    union
    {
        float    fl;
        uint32_t dw;
    } f;

    f.fl = fraw;
    Packet8[0] = (f.dw & 0xFF000000) >> 24;
    Packet8[1] = (f.dw & 0xFF0000) >> 16;
    Packet8[2] = (f.dw & 0xFF00) >> 8;
    Packet8[3] = (f.dw & 0xFF);
}

uint32_t utilites::ConvertedFloatTo32(float fraw)
{
    union
    {
        float    fl;
        uint32_t dw;
    } f;

    f.fl = fraw;
    return f.dw;
}

float utilites::Converted8_32ToFloat(uint8_t *Packet8)
{
    union
    {
        float    fl;
        uint32_t dw;
    } f;

    f.dw = ((uint8_t)Packet8[0]) << 24;
    f.dw |= ((uint8_t)Packet8[1]) << 16;
    f.dw |= ((uint8_t)Packet8[2]) << 8;
    f.dw |= ((uint8_t)Packet8[3]);
    return f.fl;
}

float utilites::Converted32ToFloat(uint32_t raw)
{
    union
    {
        float    fl;
        uint32_t dw;
    } f;

    f.dw = raw;
    return f.fl;
}

uint32_t utilites::GetLocalTime(void)
{
    time_t rawtime;
    struct tm timeinfo;
    time(&rawtime);
    timeinfo = *localtime (&rawtime);
    timeinfo.tm_hour += 9;
    timeinfo.tm_sec += 1;
    rawtime = mktime(&timeinfo);


    /*uint8_t SynchroTime[4];
    SynchroTime[0] = (rawtime & 0xFF000000) >> 24;
    SynchroTime[1] = (rawtime & 0xFF0000) >> 16;
    SynchroTime[2] = (rawtime & 0xFF00) >> 8;
    SynchroTime[3] = (rawtime & 0xFF);*/
    return (uint32_t)rawtime;
}
