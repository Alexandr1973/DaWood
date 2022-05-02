#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include "utilites.h"
#include "tblconfig.h"



#define TBLCONFIG_LENGTH    50
typedef struct
{
    char serial_num[12];
    char hw_rev[4];
    char hw_ver[6];
    char sw_ver[11];
    char holeuid[8];
    char registraion[4];
}dev_info;

typedef struct
{
    uint16_t voltage;
    uint8_t soc;
}battery_state;

typedef struct
{
    float voltage;
    float current;
    float power;
}sensor_pwrblock;

typedef struct
{
    uint32_t thermo_size;
    uint32_t piezo_size;
    uint32_t inklin_size;
    uint32_t devlogs_size;
    uint32_t gps_size;
    uint32_t thermo_num;
    uint32_t piezo_num;
    uint32_t inklin_num;
}datalogs_info;

typedef struct
{
    int8_t rssi;
    battery_state battery;
    int8_t t_cpu;
    int8_t t_ext;
    uint8_t num_of_sens;
    uint8_t type_s;
    int code_result;
    uint8_t ber;
    sensor_pwrblock senspwr;
}dev_status;

typedef struct
{
    uint16_t min_battery;
    uint16_t diff;
    float coef2voltage;
    uint16_t refVoltage;
}coef_adc;

class device : public tblConfig
{
public:
    device();
    void setDeviceFullInfo(uint8_t*);
    dev_info getDeviceInfo(void);
    dev_status getDeviceStatus(void);
    datalogs_info getDataLogsInfo(void);
    coef_adc getADCCoef(void);
private:
    dev_info dev_info;
    dev_status dev_status;
    datalogs_info datalogs_info;
    coef_adc adc_coef;
};

#endif // DEVICE_H
