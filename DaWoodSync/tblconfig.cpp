#include "tblconfig.h"

tblConfig::tblConfig()
{

}

tblConfig::tblConfig(rangeParamaters v)
{
    range = v;
}

void tblConfig::unPacking(uint8_t* cfgraw)
{
    cfgfile.freq_req 			= (uint16_t)(((uint8_t)cfgraw[0] << 8) | (uint8_t)cfgraw[1]);

    cfgfile.critical_value      = (uint8_t)cfgraw[2];

    cfgfile.flag_syncdlogs      = (bool)((uint8_t)cfgraw[3] & 0x01);
    cfgfile.flag_upd 			= (bool)(((uint8_t)cfgraw[3] & 0x02) >> 1);
    cfgfile.operating_mode 			= (OperationModeType)(((uint8_t)cfgraw[3] & 0xFC) >> 2);

    cfgfile.dt_mode 			= (bool)((uint8_t)cfgraw[4] & 0x01);
    cfgfile.dom_mode 			= (bool)(((uint8_t)cfgraw[4] & 0x02) >> 1);
    cfgfile.c2s.mode              = (Connection2ServerModeType)(((uint8_t)cfgraw[4] & 0xFC) >> 2);

    cfgfile.c2s.freq_export 		= (uint16_t)(((uint8_t)cfgraw[5] << 8) | (uint8_t)cfgraw[6]);

    cfgfile.c2s.date_time 			= Convert8to32bits(cfgraw + 7);	//Date and time connection to server

    memcpy(cfgfile.mobileNum1, (uint8_t*)cfgraw + 11, 12);

    cfgfile.c2s.nb_of_try         = (uint8_t)cfgraw[11 + 12];

    cfgfile.c2s.try_period       = Convert8to16bits(cfgraw + 11 + 12 + 1);
    cfgfile.c2s.min_battery      = (uint8_t)cfgraw[26];
    cfgfile.c2s.ipaddr = QString::fromLocal8Bit((char*)cfgraw + 27, 14);
    cfgfile.c2s.ipport = QString::fromLocal8Bit((char*)cfgraw + 27 + 15, 5);
    cfgfile.c2s.min_temperature = (int8_t)cfgraw[27 + 15 + 5];
    uint16_t tmp = Convert8to16bits(cfgraw + 27 + 15 + 5 + 1);
    cfgfile.isStartTimeReq = (tmp & 0x8000) == 0x8000;
    QTime time2;
    time2.setHMS((tmp & 0x7C0) >> 6, (tmp & 0x3f), 0, 0);
    cfgfile.StartTimeReq = time2;
}

void tblConfig::Packing(cfg_file cfg, uint8_t* outdata)
{
    QByteArray tmp;
    Convert16to8bits(outdata, cfg.freq_req);

    outdata[2]          = cfg.critical_value;

    outdata[3]          = ((uint8_t)cfg.flag_syncdlogs & 0x01);
    outdata[3] 			|= (((uint8_t)cfg.flag_upd) << 1);
    outdata[3]			|= (((uint8_t)cfg.operating_mode) << 2);

    outdata[4]          = (uint8_t)cfg.c2s.mode << 2;

    Convert16to8bits(outdata + 5, cfg.c2s.freq_export);
    Convert32to8bits(outdata + 7, cfg.c2s.date_time);

    memcpy(outdata + 11, cfg.mobileNum1, 12);//23

    outdata[11 + 12]    = (uint8_t)cfg.c2s.nb_of_try;

    Convert16to8bits(outdata + 11 + 12 + 1, cfg.c2s.try_period);//26
    outdata[26]         = (uint8_t)cfg.c2s.min_battery;
    tmp = cfg.c2s.ipaddr.toLocal8Bit();
    for (int i = 0 ; i < 21; i++)
        qDebug() << tmp.constData()[i];
    memcpy(outdata + 27, tmp.constData(), 15);
    tmp = cfg.c2s.ipport.toLocal8Bit();
    memcpy(outdata + 27 + 15, tmp.constData(), 5);
    outdata[27 + 15 + 5] = cfg.c2s.min_temperature;

    uint16_t tmp2 = (cfg.StartTimeReq.hour() << 6) & 0x7C0;
    tmp2 |= cfg.StartTimeReq.minute() & 0x3F;
    tmp2 |= cfg.isStartTimeReq ? 0x8000 : 0x00;
    Convert16to8bits(outdata + 27 + 15 + 5 + 1, tmp2);
}

tblConfig::cfg_file tblConfig::getTblConfig()
{
    return cfgfile;
}

uint16_t tblConfig::getFrequencyRequestBraid()
{
    return cfgfile.freq_req;
}

uint8_t tblConfig::get_c2sTryPeriod()
{
    return cfgfile.c2s.try_period;
}

uint8_t tblConfig::get_c2sNumberOfTry()
{
    return cfgfile.c2s.nb_of_try;
}

void tblConfig::setIsStartTimeReq(bool start)
{
    cfgfile.isStartTimeReq = start;
}

void tblConfig::setStartTimeReq(QTime time)
{
    cfgfile.StartTimeReq = time;
}

void tblConfig::setFreqReqSenors(uint16_t freq_min)
{
//    if ((freq_min > range.MIN_MINUTE_FREQ_REQ_SENSORS) || (freq_min > range.MAX_MINUTE_FREQ_REQ_SENSORS))
    cfgfile.freq_req = freq_min;
}

void tblConfig::setCriticalValue(uint8_t percent)
{
    cfgfile.critical_value = percent;
}

void tblConfig::setDeviceLogsEnabled(bool devlog)
{
    cfgfile.flag_syncdlogs = devlog;
}

void tblConfig::setOperationMode(OperationMode opermode)
{
    cfgfile.operating_mode = opermode;
}

void tblConfig::setC2Smode(Connection2ServerMode c2s_mode)
{
    cfgfile.c2s.mode = c2s_mode;
}

void tblConfig::setC2SMinutesModeFreq(uint16_t freq_export)
{
    cfgfile.c2s.freq_export = freq_export;
}

void tblConfig::setC2SDateTime(uint32_t datetime)
{
    cfgfile.c2s.date_time = datetime;
}

void tblConfig::setMobileTelephoneNumber1(char* tel_num)
{
    memcpy(cfgfile.mobileNum1, tel_num, 12);
}

void tblConfig::setC2SNumberOfTry(uint8_t numoftry)
{
    cfgfile.c2s.nb_of_try = numoftry;
}

void tblConfig::setC2STryPeriod(uint16_t tryperiod_minute)
{
    cfgfile.c2s.try_period = tryperiod_minute;
}

void tblConfig::setC2SMinimumBattery(uint8_t percent)
{
    cfgfile.c2s.min_battery = percent;
}

void tblConfig::setC2SMinimumTemperature(int8_t temperature)
{
    cfgfile.c2s.min_temperature = temperature;
}

void tblConfig::setIPAddrServer(QString ipaddr)
{
    cfgfile.c2s.ipaddr = ipaddr;
}

void tblConfig::setPortServer(QString ipport)
{
   cfgfile.c2s.ipport = ipport;
}
