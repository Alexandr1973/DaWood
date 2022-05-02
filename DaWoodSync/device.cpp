#include "device.h"

device::device()
{

}

void device::setDeviceFullInfo(uint8_t *devdata)
{
    memcpy(dev_info.serial_num, devdata + 10, 12);
    memcpy(dev_info.hw_rev, devdata, 4);
    memcpy(dev_info.hw_ver, devdata + 4, 6);
    memcpy(dev_info.sw_ver, devdata + 10 + 12, 11);

    memcpy(dev_info.holeuid, devdata + 10 + 12 + 11 + 2 + 1 + 1 + 1 + 1 + 4 + 4 + 4 + 4 + 4 + 4, 8);

    dev_status.battery.voltage = Convert8to16bits(devdata + 10 + 12 + 11);
    dev_status.battery.soc =            devdata[10 + 12 + 11 + 2];
    dev_status.t_ext =                   devdata[10 + 12 + 11 + 2 + 1];

    dev_status.num_of_sens =            devdata[10 + 12 + 11 + 2 + 1 + 1];
    dev_status.type_s =                 devdata[10 + 12 + 11 + 2 + 1 + 1 + 1];

    dev_status.senspwr.voltage = Converted8_32ToFloat(devdata + 10 + 12 + 11 + 2 + 1 + 1 + 1 + 1 + 4 + 4 + 4 + 4 + 4 + 4 + 8 + 4);
    dev_status.senspwr.current = Converted8_32ToFloat(devdata + 10 + 12 + 11 + 2 + 1 + 1 + 1 + 1 + 4 + 4 + 4 + 4 + 4 + 4 + 8 + 4 + 4);
    dev_status.senspwr.power = Converted8_32ToFloat(devdata + 10 + 12 + 11 + 2 + 1 + 1 + 1 + 1 + 4 + 4 + 4 + 4 + 4 + 4 + 8 + 4 + 4 + 4);
    dev_status.t_cpu =                   devdata[10 + 12 + 11 + 2 + 1 + 1 + 1 + 1 + 4 + 4 + 4 + 4 + 4 + 4 + 8 + 4 + 4 + 4 + 4 + 1];

    datalogs_info.thermo_size =            Convert8to32bits(devdata + 10 + 12 + 11 + 2 + 1 + 1 + 1 + 1);
    datalogs_info.piezo_size =             Convert8to32bits(devdata + 10 + 12 + 11 + 2 + 1 + 1 + 1 + 1 + 4);
    datalogs_info.inklin_size =            Convert8to32bits(devdata + 10 + 12 + 11 + 2 + 1 + 1 + 1 + 1 + 4 + 4);

    datalogs_info.thermo_num =            Convert8to32bits(devdata + 10 + 12 + 11 + 2 + 1 + 1 + 1 + 1 + 4 + 4 + 4);
    datalogs_info.piezo_num =             Convert8to32bits(devdata + 10 + 12 + 11 + 2 + 1 + 1 + 1 + 1 + 4 + 4 + 4 + 4);
    datalogs_info.inklin_num =            Convert8to32bits(devdata + 10 + 12 + 11 + 2 + 1 + 1 + 1 + 1 + 4 + 4 + 4 + 4 + 4);
    datalogs_info.devlogs_size =			Convert8to32bits(devdata + 10 + 12 + 11 + 2 + 1 + 1 + 1 + 1 + 4 + 4 + 4 + 4 + 4 + 4 + 8);
    unPacking(devdata + 89);

    memcpy(dev_info.registraion, devdata + 10 + 12 + 11 + 2 + 1 + 1 + 1 + 1 + 4 + 4 + 4 + 4 + 4 + 4 + 8 + 4 + 4 + 4 + 4 + 1 + 1 + TBLCONFIG_LENGTH, 4);
    adc_coef.min_battery = Convert8to16bits(devdata + 10 + 12 + 11 + 2 + 1 + 1 + 1 + 1 + 4 + 4 + 4 + 4 + 4 + 4 + 8 + 4 + 4 + 4 + 4 + 1 + 1 + TBLCONFIG_LENGTH + 4);
    adc_coef.diff = Convert8to16bits(devdata + 10 + 12 + 11 + 2 + 1 + 1 + 1 + 1 + 4 + 4 + 4 + 4 + 4 + 4 + 8 + 4 + 4 + 4 + 4 + 1 + 1 + TBLCONFIG_LENGTH + 4 + 2);
    adc_coef.coef2voltage = Converted8_32ToFloat(devdata + 10 + 12 + 11 + 2 + 1 + 1 + 1 + 1 + 4 + 4 + 4 + 4 + 4 + 4 + 8 + 4 + 4 + 4 + 4 + 1 + 1 + TBLCONFIG_LENGTH + 4 + 2 + 2);
    adc_coef.refVoltage = Convert8to16bits(devdata + 10 + 12 + 11 + 2 + 1 + 1 + 1 + 1 + 4 + 4 + 4 + 4 + 4 + 4 + 8 + 4 + 4 + 4 + 4 + 1 + 1 + TBLCONFIG_LENGTH + 4 + 8);
}

dev_info device::getDeviceInfo(void)
{
    return dev_info;
}

dev_status device::getDeviceStatus(void)
{
    return dev_status;
}

datalogs_info device::getDataLogsInfo(void)
{
    return datalogs_info;
}

coef_adc device::getADCCoef(void)
{
    return adc_coef;
}
