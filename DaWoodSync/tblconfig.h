#ifndef TBLCONFIG_H
#define TBLCONFIG_H

#include <QObject>
#include "utilites.h"

class tblConfig : public utilites
{
public:
    enum OperationModeType
    {
        Remote = 0x00,
        Cumulative = 0x01,
        Hand = 0x02
    };
    typedef OperationModeType OperationMode;

    enum Connection2ServerModeType
    {
        Minutes = 0x00,
        Everyday = 0x01,
        Everyweek = 0x02,
        Everymonth = 0x03
    };
    typedef Connection2ServerModeType Connection2ServerMode;

    typedef enum WeekDayType
    {
        MONDAY = 0x01,
        TUESDAY = 0x02,
        WEDNESDAY,
        THURSDAY,
        FRIDAY,
        SATURDAY,
        SUNDAY
    }WeekDay;

    typedef struct eWeekConfigType
    {
        WeekDay eWeekDay;
        uint8_t hour;
        uint8_t minute;
    }eWeekStr;

    struct Connection2ServerType
    {
        Connection2ServerMode mode :6;
        uint32_t date_time;
        uint8_t nb_of_try;
        uint16_t try_period;
        uint8_t min_battery;
        int8_t min_temperature;
        uint16_t freq_export;
        QString ipaddr;
        QString ipport;
        eWeekStr eWeek;
    };

    typedef Connection2ServerType Connection2Server;

    struct tblcfg
    {
        bool isStartTimeReq;
        QTime StartTimeReq;
        uint16_t freq_req;
        uint8_t critical_value;
        bool flag_syncdlogs :1;
        bool flag_upd:1;
        OperationMode operating_mode:6;
        bool dt_mode:1;
        bool dom_mode:1;
        char mobileNum1[12];
        Connection2Server c2s;
    };

    typedef tblcfg cfg_file;

    struct rangeForCfgParameters
    {
        uint16_t MIN_MINUTE_FREQ_REQ_SENSORS;
        uint16_t MAX_MINUTE_FREQ_REQ_SENSORS;
    };

    typedef rangeForCfgParameters rangeParamaters;
    tblConfig();
    tblConfig(rangeParamaters v);
    void unPacking(uint8_t* cfgraw);
    void Packing(cfg_file cfg, uint8_t* outdata);
    cfg_file getTblConfig();
    uint16_t getFrequencyRequestBraid();
    uint8_t get_c2sTryPeriod();
    uint8_t get_c2sNumberOfTry();

    void setIsStartTimeReq(bool start);
    void setStartTimeReq(QTime time);
    void setFreqReqSenors(uint16_t freq_min = 240);
    void setCriticalValue(uint8_t percent = 0);
    void setDeviceLogsEnabled(bool devlog = true);
    void setOperationMode(OperationMode opermode = OperationMode::Remote);
    void setC2Smode(Connection2ServerMode c2s_mode = Connection2ServerMode::Everyday);
    void setC2SMinutesModeFreq(uint16_t freq_export = 10);
    void setC2SDateTime(uint32_t datetime);
    void setMobileTelephoneNumber1(char* tel_num);
    void setC2SNumberOfTry(uint8_t numoftry = 5);
    void setC2STryPeriod(uint16_t tryperiod_minute = 120);
    void setC2SMinimumBattery(uint8_t percent = 16);
    void setC2SMinimumTemperature(int8_t temperature = -35);
    void setIPAddrServer(QString ipaddr);
    void setPortServer(QString ipport);

private:

    rangeParamaters range;
    cfg_file cfgfile;
};

#endif // TBLCONFIG_H
