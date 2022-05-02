#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <windows.h>
#include "crc32.h"
#include "myserialport.h"
#include "device.h"
#include "thermo.h"
#include "piezo.h"
#include "inklin.h"
#include "devlogs.h"
#include <cmath>
#include "firmware_read.h"
enum stepConnection
{
    NOTHING_DO = 0,
    WAIT_TIME_SEARCH_DEVICE,
    WAIT_ANSWER_OK,
    SEARCH_MY_DEVICE,
    SEND_AND_RECEIVE_INFO,
    SYNC_INIT,
    SYNCHRONIZATION_DATA_RECEIVE,
    SYNCHRONIZATION_DATA_PROCESSING,
    SYNCHRONIZATION_DATETIME,
    REQUEST_FORMAT_FLASH,
    REQUEST_FIRMWARE_UPDATE,
    FIRMWARE_UPDATE_PRESTART,
    FIRMWARE_UPDATE_START,
    SEND_CONFIG_FILE,
    SEND_ONLINE_DATA_START,
    REQUEST_ONLINE_DATA,
    WAIT_ONLINE_DATA,
    SEND_ONLINE_DATA_STOP,
    REQUEST_FORMAT_FLASH_SERVICE
};



class DataProcessing : public QObject, utilites, crc32, firmware_read
{
    Q_OBJECT
public:

    enum AutomationReceiveDataLogsType
    {
        CHECK_THERMODATA    = 0,
        RECEIVE_THERMODATA  = 1,
        CHECK_PIEZODATA     = 2,
        RECEIVE_PIEZODATA   = 3,
        CHECK_INKLINDATA    = 4,
        RECEIVE_INKLINDATA  = 5,
        CHECK_DEVLOGSDATA   = 6,
        RECEIVE_DEVLOGSDATA = 7
    };
    typedef AutomationReceiveDataLogsType AutomationReceiveDataLogs;

    DataProcessing(uint32_t sizebuffer, MySerialPort *SerialPort);
    void setProcessStart(bool t);
    void setStep(stepConnection step);
    void setPathForDatalogs(QStringList list);
    void setPathFirmwareFile(QString path);
    int toPercent(void);
    void giveMeTblConfig(tblConfig::cfg_file cfgf);
    device* dev;
    inklin::inklinData inklinOnline[200];
    uint8_t inklinOnlineLength;
public slots:
    void proccessing();
    void ReceiveCurrentNumberOfProcess(int);
signals:
    void DataIsReady();
    void OnlineDataIsReady();
    void ErrorOccured(int);
    void finished();
    void SendCurrentNumberOfProcess(int);
    void SendCurrentPercentProcess(float);
    void SendCurrentFirmwarePercent(float);
    void SendFirmwareUpdatedStart();
    void SendFirmwareUpdatedEnd();
    void isComplete(int operation);
    void Send2SerialPort(char* data, unsigned char length);
    void SendCurrentDownloadDatalogs(int);
private:
    int ReceiveDataLogs(QByteArray* buffer, uint32_t length);
    bool isValidCRCData(uint8_t* data, uint32_t length);
    int Timeout(int process);
    MySerialPort *SerialPort;
    stepConnection step;
    stepConnection PrevStep;
    uint32_t timeout;
    uint32_t countTimeout;
    uint32_t length_firmware, iteration_firmware, proportion_firmware, iteration_now;
    bool isUpdatedFirmware;
    uint8_t *firmwarefile;
    float _proportion_firmware;
    crc32 crc;
    uint32_t crcres,crcres2;
    bool startThread;
    uint32_t rawtime;
    QStringList pathForDatalogs;
    QString pathFirmwareFile;
    uint32_t SIZE_BUFFER_INTO_DEVICE;
    uint32_t CurrentDatalogsLength;
    int processing_step;
    QByteArray buffer4thermo, buffer4piezo, buffer4devlogs;
    thermo* thermodata;
    piezo* piezodata;
    devlogs *devlogsdata;
    int fullNumberOfProcessing;
    int currentNumberOfProcessing;
    AutomationReceiveDataLogs step_receive_datalogs;
    bool waitanswertimeout;
};

#endif // DATAPROCESSING_H
