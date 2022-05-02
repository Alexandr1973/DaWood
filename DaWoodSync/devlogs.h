#ifndef DEVLOGS_H
#define DEVLOGS_H

#include <QObject>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <QDebug>
#include <QTemporaryDir>
#include <QDateTime>
#include <iomanip>
#include "utilites.h"

#define DEBUG_MESSAGE_TYPE_SYSTEM				0
#define DEBUG_MESSAGE_TYPE_APPLICATIONS			1
#define DEBUG_MESSAGE_TYPE_SOCKET_WIRELESS		2
#define DEBUG_MESSAGE_TYPE_SOCKET_SENSORS		3
#define DEBUG_MESSAGE_TYPE_SOCKET_DEBUG			4
#define DEBUG_MESSAGE_NUMERIC_EMPTY   			0x80000000

const char code_error_decrypt[][50] =
{
    "Ready to standby mode",		//1
    "USB Detected value",			//2
    "Initialization main end",		//3
    "Battery soc",					//4
    "FlagRegisteredIntoServer",		//5
    "Device_id",					//6
    "PIEZO_SIZE",					//7
    "THERMO_SIZE",					//8
    "INKLIN_SIZE",					//9
    "LL_PWR_IsActiveFlag_WU5",		//10
    "LL_PWR_IsActiveFlag_InternWU",	//11
    "LL_RTC_IsActiveFlag_ALRA",		//12
    "LL_RTC_IsActiveFlag_ALRB",		//13
    "FLAG_START_SAVEENERGY",		//14
    "LL_RCC_IsActiveFlag_HSECSS",	//15
    "LL_RCC_IsActiveFlag_LSECSS",	//16
    "LL_RCC_IsActiveFlag_SFTRST",	//17
    "LL_RCC_IsActiveFlag_PINRST",	//18
    "DeviceOperationMode",			//19
    "TaskFromAlarms",				//20
    "NMI & HardFault",				//21
    "Protocol_SendHeader",			//22
    "Data length location",			//23
    "TotalSizePacket",				//24
    "Iteration",					//25
    "Propotion",					//26
    "SyncCount",					//27
    "Read Iteration",				//28
    "Read Propotion",				//29
    "Thermo data empty",			//30
    "Thermo data sent",				//31
    "Piezo data empty",				//32
    "Piezo data sent",				//33
    "sim800_SocketSendMessage",		//34
    "sim800_SocketSendMessage Error",	//35
    "Memory formatting",			//36
    "tblConfig_IsUpdateFirmware",	//37
    "Inklin data empty",			//38
    "Inklin data sent",				//39
    "System logs empty",			//40
    "System logs sent",				//41
    "Inklinometr. Write data end",	//42
    "Inklinometr. Timeout request",	//43
    "Inklinometr. Sorting start",	//44
    "Inklinometr. Request start",	//45
    "Search iButton",				//46
    "Not Detected iButton",			//47
    "Hole UID read from memory",	//48
    "Detected iButton",				//49
    "Thermo",						//50
    "Piezo connected",				//51
    "Battery is low",					//52
    "Write datalogs",				//53
    "Thermo",						//54
    "Piezo connected",				//55
    "Write datalogs",				//56
    "Reset CPU",					//57
    "GsmNotAvailable",				//58
    "ServerNotAvailable",			//59
    "cnt_try_c2s",					//60
    "freq_req",						//61
    "alarm hour",					//62
    "alarm min",					//63
    "alarm sec",					//64
    "alarm server hour",			//65
    "alarm server min",				//66
    "alarm server sec",				//67
    "Firmware download start",		//68
    "SECTOR_FIRMWARE_LENGTH_LOCATION",	//69
    "ReceiveFirmwareIteration",			//70
    "ReceiveFirmwarePropotion",			//71
    "ReceiveLength",					//72
    "ReceiveFirmwareCount",				//73
    "Firmware download end",			//74
    "startup",							//75
    "gprs",								//76
    "socket",							//77
    "sim800_ReceptionHandler",			//78
    "sim800_SocketReceiveMessage",		//79
    "ButtonMenu short",					//80
    "ButtonMenu long",					//81
    "ButtonArrow long",					//82
    "ButtonArrow short",				//83
    "Start program",					//84
    "System log enable",				//85
    "EnterStandbyMode",					//86
    "Battery voltage",						//87
    "LED_GREEN_ON",						//88
    "LED_GREEN_OFF",						//89
    "Program in progress",				//90
    "t_cpu",				//91
    "SOCKET_SENSORS_PWR_ENABLE",			//92
    "SOCKET_WIRELESS_PWR_ENABLE",			//93
    "SOCKET_SENSORS_PWR_DISABLE",			//94
    "SOCKET_WIRELESS_PWR_DISABLE",			//95
    "OperationModeTask",					//96
    "LL_RCC_IsActiveFlag_BORRST",			//97
    "t_ext",								//98
    "RestoreData. Propotion",				//99
    "RestoreData. LastSizeValue",			//100
    "RestoreData. curraddr_backupdata",		//101
    "RestoreData. curraddr_currentdata",	//102
    "RestoreData. lengthData",				//103
    "RestoreData. crc1 != crc2",			//104
    "CheckData. LastSizeValue",				//105
    "CheckData. lengthData",				//106
    "CheckData. crc1 != crc2",				//107
    "BackupData. crc1 != crc2"				//108
};

class devlogs : public QObject, utilites
{
    Q_OBJECT
public:
    devlogs();
    int packageProcessing(QString pathdatalogs, uint8_t* bufferData, uint32_t length, int device_id);

signals:
        void sendCurrentNoFProcess(int);
};

#endif // DEVLOGS_H
