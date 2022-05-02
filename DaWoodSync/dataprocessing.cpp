#include "dataprocessing.h"

static uint8_t SearchMyDevice[] = {0x01,0x02,0x03,0x04,0x05};
static uint8_t SyncRTC_ByteArray[7 + 4 + 4] = {"rtcsync"};
static uint8_t AnswerOk[] = "crc ok";
static uint8_t FormatFlash_ByteArray[] = {"formatflash"};
static uint8_t FormatFlash2_ByteArray[] = {"servicerase"};
static uint8_t SendLengthFirmware_ByteArray[6 + 4 + 2 + 2 + 4] = "length";
static uint8_t SendFirmware_ByteArray[4 + 4 + 52 + 4] = "fwit";
static uint8_t OnlineDataStart_ByteArray[] = "online";
static uint8_t OnlineDataStop_ByteArray[] = "onstop";
static uint8_t OnlineDataInklin_ByteArray[] = "oninklin";
static uint8_t cmd_SyncDataLog_Thermo[10] = {'s','y','n','c','d','a','t','a',0x01};
static uint8_t cmd_SyncDataLog_Piezo[10] = {'s','y','n','c','d','a','t','a',0x02};
static uint8_t cmd_SyncDataLog_Inklin[10] = {'s','y','n','c','d','a','t','a',0x03};
static uint8_t cmd_SyncDataLog_Devlogs[10] = {'s','y','n','c','d','a','t','a',0x04};

const uint8_t SyncConfigFile_ByteArray[] = "synccfg";
uint8_t buffer[200];
DataProcessing::DataProcessing(uint32_t sizebuffer, MySerialPort *SerialPort)
{
    this->SIZE_BUFFER_INTO_DEVICE = sizebuffer;
    this->SerialPort = SerialPort;
    startThread = true;
    dev = new device;
    thermodata = new thermo;
    piezodata = new piezo;
    devlogsdata = new devlogs;
    timeout = 0;
    countTimeout = 0;
    step = NOTHING_DO;
    PrevStep = NOTHING_DO;
    currentNumberOfProcessing = 0;
    iteration_firmware = 0;
    length_firmware = 0;
    proportion_firmware = 0;
    iteration_now = 0;
    isUpdatedFirmware = false;
    waitanswertimeout = false;
    connect(this, SIGNAL(SendCurrentNumberOfProcess(int)), this, SLOT(ReceiveCurrentNumberOfProcess(int)));
    connect(thermodata, SIGNAL(sendCurrentNoFProcess(int)), this, SLOT(ReceiveCurrentNumberOfProcess(int)), Qt::DirectConnection);
    connect(piezodata, SIGNAL(sendCurrentNoFProcess(int)), this, SLOT(ReceiveCurrentNumberOfProcess(int)), Qt::DirectConnection);
    connect(devlogsdata, SIGNAL(sendCurrentNoFProcess(int)), this, SLOT(ReceiveCurrentNumberOfProcess(int)), Qt::DirectConnection);
}

void DataProcessing::ReceiveCurrentNumberOfProcess(int n)
{
    currentNumberOfProcessing += n;
    float percent = 100 / ((float)((float)fullNumberOfProcessing) / currentNumberOfProcessing);
    emit SendCurrentPercentProcess(percent);
//    qDebug() << "Full no process: " << fullNumberOfProcessing;
//    qDebug() << "Current no process: " << currentNumberOfProcessing;
//    qDebug() << "Current percent: " << percent << "%";
}

void DataProcessing::setPathForDatalogs(QStringList list)
{
    this->pathForDatalogs = list;
}

void DataProcessing::setPathFirmwareFile(QString path)
{
    pathFirmwareFile = path;
}

void DataProcessing::setStep(stepConnection step)
{
    this->step = step;
}

void DataProcessing::setProcessStart(bool t)
{
    this->startThread = t;
}

int DataProcessing::toPercent(void)
{
    uint32_t length = dev->getDataLogsInfo().thermo_size;
    unsigned int iteration, proportion;
    unsigned int fullnoprocess;
    float _proportion;
    iteration = length / SIZE_BUFFER_INTO_DEVICE;
    _proportion = (float)(((float)((float)length / SIZE_BUFFER_INTO_DEVICE) - iteration) * SIZE_BUFFER_INTO_DEVICE);
    proportion = (!iteration) ? length : (unsigned int)round(_proportion);
    if (proportion) iteration += 1;
    fullnoprocess = iteration * 200;
    qDebug() << "thermo_size: " << iteration;

    length = dev->getDataLogsInfo().piezo_size;
    iteration = length / SIZE_BUFFER_INTO_DEVICE;
    _proportion = (float)(((float)((float)length / SIZE_BUFFER_INTO_DEVICE) - iteration) * SIZE_BUFFER_INTO_DEVICE);
    proportion = (!iteration) ? length : (unsigned int)round(_proportion);
    if (proportion) iteration += 1;
    fullnoprocess += iteration * 200;
    qDebug() << "piezo_size: " << iteration;

    length = dev->getDataLogsInfo().inklin_size;
    iteration = length / SIZE_BUFFER_INTO_DEVICE;
    _proportion = (float)(((float)((float)length / SIZE_BUFFER_INTO_DEVICE) - iteration) * SIZE_BUFFER_INTO_DEVICE);
    proportion = (!iteration) ? length : (unsigned int)round(_proportion);
    if (proportion) iteration += 1;
    fullnoprocess += iteration * 200;
    qDebug() << "inklin_size: " << iteration;

    length = dev->getDataLogsInfo().devlogs_size;
    iteration = length / SIZE_BUFFER_INTO_DEVICE;
    _proportion = (float)(((float)((float)length / SIZE_BUFFER_INTO_DEVICE) - iteration) * SIZE_BUFFER_INTO_DEVICE);
    proportion = (!iteration) ? length : (unsigned int)round(_proportion);
    if (proportion) iteration += 1;
    fullnoprocess += iteration * 200;
    qDebug() << "devlogs_size: " << iteration;

    iteration = (dev->getDataLogsInfo().thermo_num * 300) + dev->getDataLogsInfo().piezo_num + dev->getDataLogsInfo().inklin_num +
            (dev->getDataLogsInfo().devlogs_size / 12);
//    iteration /= 1000;
    qDebug() << "all_size: " << iteration;
    fullnoprocess += iteration;
    return fullnoprocess;
}

bool DataProcessing::isValidCRCData(uint8_t* data, uint32_t length)
{
    crcres2 = Convert8to32bits(data + length);
    crcres = CRC32(data, length);
//            qDebug() << "CRC: " << crcres  << "CRC rec: " << crcres2;
    if (crcres == crcres2)
    {
        qDebug() << QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss") << "CRC is valid";
//        SerialPort->sendData((char*)AnswerOk, 6);
        emit Send2SerialPort((char*)AnswerOk, 6);
        return true;
    }
    qDebug() << QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss") << "CRC is invalid";
    return false;
}

int DataProcessing::ReceiveDataLogs(QByteArray* buffer, uint32_t length)
{
    unsigned int iteration, proportion;
    float _proportion;
    int count = 0;

    if (!length) return -3;
//    qDebug() << "Start ReceiveDataLogs";
    iteration = length / SIZE_BUFFER_INTO_DEVICE;
    _proportion = (float)(((float)((float)length / SIZE_BUFFER_INTO_DEVICE) - iteration) * SIZE_BUFFER_INTO_DEVICE);
    proportion = (!iteration) ? length : (unsigned int)round(_proportion);

//    qDebug() << "iteration: " << iteration;
//    qDebug() << "proportion: " << proportion;

    for (unsigned int i = 0; i < iteration; i++)
    {

        for (unsigned int j = 0; j < 3; j++)
        {
            ++count;
            while(SerialPort->getData().isEmpty() == true) Sleep(100);  //TODO: НУжно поставить таймаут. Виснет если что то пошло не так.
            if (isValidCRCData((uint8_t*)SerialPort->getData().constData(), SIZE_BUFFER_INTO_DEVICE) == true)
            {
                emit SendCurrentNumberOfProcess(200);
                buffer->append(SerialPort->getData().constData(), SIZE_BUFFER_INTO_DEVICE);
                count = 0;
                break;
            }
            SerialPort->clearData();
        }
        SerialPort->clearData();
        if (count > 2)
            return -1;
    }
    for (unsigned int j = 0; j < 3; j++)
    {
        ++count;
        while(SerialPort->getData().isEmpty() == true) Sleep(100); //TODO: НУжно поставить таймаут. Виснет если что то пошло не так.
        crcres2 = Convert8to32bits((uint8_t*)SerialPort->getData().constData() + proportion);
        crcres = CRC32((uint8_t*)SerialPort->getData().constData(), proportion);
//        qDebug() << "CRC: " << crcres  << "CRC rec: " << crcres2;
        if (isValidCRCData((uint8_t*)SerialPort->getData().constData(), proportion) == true)
        {
            emit SendCurrentNumberOfProcess(200);
            buffer->append(SerialPort->getData().constData(), proportion);
            break;
        }

        SerialPort->clearData();

    }
    SerialPort->clearData();
    if (count > 2)
        return -2;
    qDebug() << "Stop ReceiveDataLogs";

    return 0;
}

void DataProcessing::giveMeTblConfig(tblConfig::cfg_file cfgf)
{
    dev->Packing(cfgf, buffer + 7);
    memcpy(buffer, SyncConfigFile_ByteArray, 7);
    qDebug() << QByteArray((char*)buffer, 7 + 27 + 23).toHex();
}



void DataProcessing::proccessing()
{
    while(startThread)
    {
        switch(step)
        {
        case NOTHING_DO:
            break;

        case WAIT_TIME_SEARCH_DEVICE:
            if (++timeout > 200)    //4 sec
            {
                step = SEARCH_MY_DEVICE;
                timeout = 0;
            }
            break;

        case SEARCH_MY_DEVICE:
            emit Send2SerialPort((char*)SearchMyDevice, 5);
//            SerialPort->sendData((char*)SearchMyDevice, 5);
            step = SEND_AND_RECEIVE_INFO;
//            if (portnum >= 20) portnum = 0;
           //            portname = "COM";
           //            portname.append(QString::number(++portnum));
           //            qDebug() << portname;
           //            if (this->iSerialPort->openSerialPort(portname) == 1)
           //            {
           //                this->step = SEND_AND_RECEIVE_INFO;
           //                qDebug() << "Connected to " + portname;
           //                this->iSerialPort->sendData((char*)SearchMyDevice);
           //            }
           //            else
           //            {
           //                this->iSerialPort->closeSerialPort();
           //            }
            break;

        case SEND_AND_RECEIVE_INFO:

            if (++timeout > 100)
            {

                step = SEARCH_MY_DEVICE;
                timeout = 0;
            }

            if (SerialPort->getData().isEmpty() == false)   //Если пришли данные
            {
                if (isValidCRCData((uint8_t*)SerialPort->getData().constData(), (SerialPort->getData().length() - 4)) == true)
                {
                    dev->setDeviceFullInfo((uint8_t*)SerialPort->getData().constData());

//                    for(int i = 0; i < SerialPort->getData().length(); i++)
//                    {
//                        qDebug() << (unsigned char)SerialPort->getData().constData()[i];
//                    }
                    emit DataIsReady();

//                    step = WAIT_TIME_SEARCH_DEVICE;
                    step = NOTHING_DO;
                    timeout = 0;
                }

                SerialPort->clearData();
            }


            break;

        case SYNC_INIT:
            currentNumberOfProcessing = 0;
            fullNumberOfProcessing = toPercent();
            step_receive_datalogs = CHECK_THERMODATA;
            step = SYNCHRONIZATION_DATA_RECEIVE;
            waitanswertimeout = true;
            break;

        case SYNCHRONIZATION_DATA_RECEIVE:

//            if (++countTimeout > 5)
//            {
//                emit ErrorOccured(2);
//                countTimeout = 0;
//                step = SEND_AND_RECEIVE_INFO;
//                break;
//            }

            PrevStep = step;

            switch(step_receive_datalogs)
            {
            //Thermo
            case CHECK_THERMODATA:
                qDebug() << "CHECK_THERMODATA";
                if (!waitanswertimeout)
                {
                    step_receive_datalogs = RECEIVE_THERMODATA;
                    break;
                }
                if (dev->getDataLogsInfo().thermo_size)
                {
                    emit Send2SerialPort((char*)cmd_SyncDataLog_Thermo, 9);
                    step = WAIT_ANSWER_OK;
                }
                else
                    step_receive_datalogs = CHECK_PIEZODATA;
                qDebug() << "CHECK_THERMODATA result: " << step_receive_datalogs;
                break;

            case RECEIVE_THERMODATA:
                emit SendCurrentDownloadDatalogs(1);
                qDebug() << "RECEIVE_THERMODATA";
                if (ReceiveDataLogs(&buffer4thermo, dev->getDataLogsInfo().thermo_size) < 0)
                {
                    emit ErrorOccured(3);
                    step = SEARCH_MY_DEVICE;

                    break;
                }
                waitanswertimeout = true;
                step_receive_datalogs = CHECK_PIEZODATA;      //to Piezo receive
                break;

            //Piezo
            case CHECK_PIEZODATA:
                qDebug() << "CHECK_PIEZODATA";
                if (!waitanswertimeout)
                {
                    step_receive_datalogs = RECEIVE_PIEZODATA;
                    break;
                }
                if (dev->getDataLogsInfo().piezo_size)
                {
                    emit Send2SerialPort((char*)cmd_SyncDataLog_Piezo, 9);
                    step = WAIT_ANSWER_OK;
                }
                else
                    step_receive_datalogs = CHECK_INKLINDATA;
                break;

            case RECEIVE_PIEZODATA:
                emit SendCurrentDownloadDatalogs(2);
                qDebug() << "RECEIVE_PIEZODATA";
                if (ReceiveDataLogs(&buffer4piezo, dev->getDataLogsInfo().piezo_size) < 0)
                {
                    emit ErrorOccured(4);
                    step = SEARCH_MY_DEVICE;
                    break;
                }
                waitanswertimeout = true;
                step_receive_datalogs = CHECK_INKLINDATA;      //to Inklin receive
                break;

            //Inklin
            case CHECK_INKLINDATA:
                qDebug() << "CHECK_INKLINDATA";
                if (!waitanswertimeout)
                {
                    step_receive_datalogs = RECEIVE_INKLINDATA;
                    break;
                }
                if (dev->getDataLogsInfo().inklin_size)
                {
                    emit Send2SerialPort((char*)cmd_SyncDataLog_Inklin, 9);
                    step = WAIT_ANSWER_OK;
                }
                else
                    step_receive_datalogs = CHECK_DEVLOGSDATA;
                break;

            case RECEIVE_INKLINDATA:
                emit SendCurrentDownloadDatalogs(3);
                qDebug() << "RECEIVE_INKLINDATA";
                step_receive_datalogs = CHECK_DEVLOGSDATA;
                break;

            //Devlogs
            case CHECK_DEVLOGSDATA:
                qDebug() << "CHECK_DEVLOGSDATA";
                if (!waitanswertimeout)
                {
                    step_receive_datalogs = RECEIVE_DEVLOGSDATA;
                    break;
                }
                if (dev->getDataLogsInfo().devlogs_size)
                {
                    emit Send2SerialPort((char*)cmd_SyncDataLog_Devlogs, 9);
                    step = WAIT_ANSWER_OK;
                }
                else
                {
                    step = SYNCHRONIZATION_DATA_PROCESSING;
                    step_receive_datalogs = CHECK_THERMODATA;
                }
                break;
            case RECEIVE_DEVLOGSDATA:
                emit SendCurrentDownloadDatalogs(4);
                qDebug() << "RECEIVE_DEVLOGSDATA";
                if (ReceiveDataLogs(&buffer4devlogs, dev->getDataLogsInfo().devlogs_size) < 0)
                {
                    emit ErrorOccured(5);
                    step = SEARCH_MY_DEVICE;
                    break;
                }
                waitanswertimeout = true;
                step_receive_datalogs = CHECK_THERMODATA;
                step = SYNCHRONIZATION_DATA_PROCESSING;
                break;

            }
            break;

        case SYNCHRONIZATION_DATA_PROCESSING:
            emit SendCurrentDownloadDatalogs(5);
            qDebug() << "Start DataLogsProcessing";
            thermodata->packageProcessing(pathForDatalogs.at(0), pathForDatalogs.at(1), (uint8_t*)buffer4thermo.constData(), dev->getDataLogsInfo().thermo_num);
            piezodata->packageProcessing(pathForDatalogs.at(0), (uint8_t*)buffer4piezo.constData(),dev->getDataLogsInfo().piezo_size);
            devlogsdata->packageProcessing(pathForDatalogs.at(0), (uint8_t*)buffer4devlogs.constData(),dev->getDataLogsInfo().devlogs_size, Convert8to32bits((uint8_t*)dev->getDeviceInfo().registraion));
            qDebug() << "Stop DataLogsProcessing";
            emit isComplete(0x01);
            step = SEARCH_MY_DEVICE;
            break;


        case SYNCHRONIZATION_DATETIME:
            if (Timeout(SYNCHRONIZATION_DATETIME) < 0)  step = SEND_AND_RECEIVE_INFO;
            PrevStep = step;
            qDebug() << "SYNCHRONIZATION_DATETIME";
            rawtime = GetLocalTime();
            Convert32to8bits((uint8_t*)SyncRTC_ByteArray + 7, rawtime);
            Convert32to8bits((uint8_t*)SyncRTC_ByteArray + 7 + 4, crc.CRC32((uint8_t*)SyncRTC_ByteArray, 7 + 4));
//            SerialPort->sendData((char*)SyncRTC_ByteArray, 7 + 4 + 4);
            emit Send2SerialPort((char*)SyncRTC_ByteArray, 7 + 4 + 4);
            step = WAIT_ANSWER_OK;
            qDebug()  << "WAIT_ANSWER_OK";
            timeout = 0;
            break;

        case WAIT_ANSWER_OK:
            if (++timeout > 150)
            {
                qDebug() << "WAIT_ANSWER_OK timeout";
                step = PrevStep;
                waitanswertimeout = true;
                timeout = 0;
                countTimeout = 0;
            }

            if (SerialPort->getData().isEmpty() == false)   //Если пришли данные
            {
                if (SerialPort->getData().startsWith("crc ok"))
                {
                    waitanswertimeout = false;
//                    qDebug() << "Answer ok";
                    timeout = 0;
                    countTimeout = 0;
                    step = SEARCH_MY_DEVICE;
                    if (PrevStep == SYNCHRONIZATION_DATETIME)
                        emit isComplete(0x02);
                    else if (PrevStep == REQUEST_FORMAT_FLASH)
                        emit isComplete(0x03);
                    else if (PrevStep == REQUEST_FORMAT_FLASH_SERVICE)
                        emit isComplete(0x03);
                    else if (PrevStep == SYNCHRONIZATION_DATA_RECEIVE)
                        step = SYNCHRONIZATION_DATA_RECEIVE;
                    else if (PrevStep == SEND_CONFIG_FILE)
                        emit isComplete(0x04);
                    else if (PrevStep == REQUEST_FIRMWARE_UPDATE)
                        step = FIRMWARE_UPDATE_PRESTART;
                    else if (PrevStep == FIRMWARE_UPDATE_START)
                        step = PrevStep;
                }
                SerialPort->clearData();
            }
            break;

        case REQUEST_FORMAT_FLASH:
            if (Timeout(REQUEST_FORMAT_FLASH) < 0)  step = SEND_AND_RECEIVE_INFO;
            PrevStep = step;
            qDebug()  << "REQUEST_FORMAT_FLASH";

            emit Send2SerialPort((char*)FormatFlash_ByteArray, 11);
            step = WAIT_ANSWER_OK;
            qDebug()  << "WAIT_ANSWER_OK";
            break;

        case REQUEST_FORMAT_FLASH_SERVICE:
            if (Timeout(REQUEST_FORMAT_FLASH_SERVICE) < 0)  step = SEND_AND_RECEIVE_INFO;
            PrevStep = step;
            qDebug()  << "REQUEST_FORMAT_FLASH_SERVICE";

            emit Send2SerialPort((char*)FormatFlash2_ByteArray, 11);
            step = WAIT_ANSWER_OK;
            qDebug()  << "WAIT_ANSWER_OK";
            break;

        case REQUEST_FIRMWARE_UPDATE:

            if (Timeout(REQUEST_FIRMWARE_UPDATE) < 0)  step = SEND_AND_RECEIVE_INFO;

            PrevStep = step;
            qDebug()  << "REQUEST_FIRMWARE_UPDATE";
            length_firmware = getLength(pathFirmwareFile);
            iteration_firmware = length_firmware / 52;
            _proportion_firmware = (float)(((float)((float)length_firmware / 52) - iteration_firmware) * 52);
            proportion_firmware = (!iteration_firmware) ? length_firmware : (unsigned int)round(_proportion_firmware);
            if (proportion_firmware) iteration_firmware += 1;

            Convert32to8bits(SendLengthFirmware_ByteArray + 6, length_firmware);
            Convert16to8bits(SendLengthFirmware_ByteArray + 6 + 4, iteration_firmware);
            Convert16to8bits(SendLengthFirmware_ByteArray + 6 + 4 + 2, proportion_firmware);
            Convert32to8bits((uint8_t*)SendLengthFirmware_ByteArray + 6 + 4 + 2 + 2, crc.CRC32((uint8_t*)SendLengthFirmware_ByteArray, 6 + 4 + 2 + 2));
            emit Send2SerialPort((char*)SendLengthFirmware_ByteArray, 6 + 4 + 2 + 2 + 4);

            step = WAIT_ANSWER_OK;
            qDebug()  << "WAIT_ANSWER_OK";
            break;

        case FIRMWARE_UPDATE_PRESTART:
            emit SendFirmwareUpdatedStart();
            qDebug()  << "firmwarefile memalloc";
            firmwarefile = new uint8_t[length_firmware + 4];
            getFirmware(pathFirmwareFile, firmwarefile);
            waitanswertimeout = true;
            step = FIRMWARE_UPDATE_START;
            break;

        case FIRMWARE_UPDATE_START:
            PrevStep = step;


//            qDebug()  << "waitanswertimeout: " << waitanswertimeout;
            if (!waitanswertimeout)
            {
                ++iteration_now;
                if (isUpdatedFirmware)
                {
                    step = SEND_AND_RECEIVE_INFO;
                    iteration_now = 0;
                    isUpdatedFirmware = false;
                    delete[] firmwarefile;
                    emit SendFirmwareUpdatedEnd();
                    break;
                }
            }

           qDebug()  << "iteration_now: " << iteration_now;
            emit SendCurrentFirmwarePercent((float)((float)(iteration_now * 100) / (iteration_firmware - 1)));
            if (iteration_now + 1 < iteration_firmware)
            {
                Convert32to8bits(SendFirmware_ByteArray + 4, iteration_now);
                memcpy(SendFirmware_ByteArray + 4 + 4, firmwarefile + (iteration_now * 52), 52);
                Convert32to8bits((uint8_t*)SendFirmware_ByteArray + 4 + 4 + 52, crc.CRC32((uint8_t*)SendFirmware_ByteArray, 4 + 4 + 52));
                emit Send2SerialPort((char*)SendFirmware_ByteArray, 4 + 4 + 52 + 4);
               qDebug()  << QByteArray((char*)SendFirmware_ByteArray, 64).toHex();

            }
            else
            {
                Convert32to8bits(SendFirmware_ByteArray + 4, iteration_now);
                memcpy(SendFirmware_ByteArray + 4 + 4, firmwarefile + (iteration_now * 52), proportion_firmware);
                Convert32to8bits((uint8_t*)SendFirmware_ByteArray + 4 + 4 + proportion_firmware, crc.CRC32((uint8_t*)SendFirmware_ByteArray, 4 + 4 + proportion_firmware));
                emit Send2SerialPort((char*)SendFirmware_ByteArray, 4 + 4 + proportion_firmware + 4);
               qDebug()  << QByteArray((char*)SendFirmware_ByteArray, 4 + 4 + proportion_firmware + 4).toHex();
                isUpdatedFirmware = true;
            }

            step = WAIT_ANSWER_OK;
            break;

        case SEND_CONFIG_FILE:
            qDebug() << "SEND_CONFIG_FILE";
            if (Timeout(SEND_CONFIG_FILE) < 0)  step = SEND_AND_RECEIVE_INFO;
            PrevStep = step;

            Convert32to8bits((uint8_t*)buffer + 7 + TBLCONFIG_LENGTH, crc.CRC32((uint8_t*)buffer, 7 + TBLCONFIG_LENGTH));
            emit Send2SerialPort((char*)buffer, 7 + TBLCONFIG_LENGTH + 4);
            step = WAIT_ANSWER_OK;
            qDebug()  << "WAIT_ANSWER_OK";
            timeout = 0;
            break;

        case SEND_ONLINE_DATA_START:
            if (Timeout(SEND_ONLINE_DATA_START) < 0)  step = NOTHING_DO;
            PrevStep = step;
            qDebug()  << "SEND_ONLINE_DATA_START";

            emit Send2SerialPort((char*)OnlineDataStart_ByteArray, 6);
            step = WAIT_ANSWER_OK;
            qDebug()  << "WAIT_ANSWER_OK";
            break;

        case SEND_ONLINE_DATA_STOP:
            if (Timeout(SEND_ONLINE_DATA_START) < 0)  step = NOTHING_DO;
            PrevStep = step;
            qDebug()  << "SEND_ONLINE_DATA_STOP";

            emit Send2SerialPort((char*)OnlineDataStop_ByteArray, 6);
            step = WAIT_ANSWER_OK;
            qDebug()  << "WAIT_ANSWER_OK";
            break;

        case REQUEST_ONLINE_DATA:
            PrevStep = step;
            qDebug()  << "REQUEST_ONLINE_DATA";

            emit Send2SerialPort((char*)OnlineDataInklin_ByteArray, 8);
            step = WAIT_ONLINE_DATA;
            break;

        case WAIT_ONLINE_DATA:

            if (++timeout > 100)
            {

                step = REQUEST_ONLINE_DATA;
                timeout = 0;
                countTimeout = 0;
            }

            if (SerialPort->getData().isEmpty() == false)   //Если пришли данные
            {
                if (isValidCRCData((uint8_t*)SerialPort->getData().constData(), (SerialPort->getData().length() - 4)) == true)
                {
//                    memcpy((uint8_t*)inklinOnline, SerialPort->getData().constData() + 8 + 1, SerialPort->getData().constData()[8] * (8 + 6));
//                    qDebug() << QByteArray(SerialPort->getData().constData(), SerialPort->getData().length()).toHex();
                    inklinOnlineLength = SerialPort->getData().constData()[8];
                    for (int i = 0; i < inklinOnlineLength; i++)
                    {
                        memcpy(inklinOnline[i].serialnum, SerialPort->getData().constData() + 9 + (i * 14), 8);
                        inklinOnline[i].axisX = ((int16_t)((int16_t)SerialPort->getData().constData()[17 + (i * 14)] << 8) | (int16_t)SerialPort->getData().constData()[18 + (i * 14)]);
                        inklinOnline[i].axisY = ((int16_t)((int16_t)SerialPort->getData().constData()[19 + (i * 14)] << 8) | (int16_t)SerialPort->getData().constData()[20 + (i * 14)]);
                        inklinOnline[i].axisZ = ((int16_t)((int16_t)SerialPort->getData().constData()[21 + (i * 14)] << 8) | (int16_t)SerialPort->getData().constData()[22 + (i * 14)]);
//                        qDebug() << "Serial: " << QByteArray((char*)inklinOnline[i].serialnum, 8).toHex();
//                        qDebug() << "X: " << inklinOnline[i].axisX;
//                        qDebug() << "Y: " << inklinOnline[i].axisY;
//                        qDebug() << "Z: " << inklinOnline[i].axisZ;

                    }
                    emit OnlineDataIsReady();
                    countTimeout = 0;
                    step = NOTHING_DO;
                    timeout = 0;
                }

                SerialPort->clearData();
            }


            break;
        }
        Sleep(20);
    }
    qDebug() << "Thread stoped";
    SerialPort->clearData();
    SerialPort->closeSerialPort();
    emit finished();
}


int DataProcessing::Timeout(int process)
{
    int res = 0;
    if (++countTimeout > 100)
    {
        emit ErrorOccured(process);
        countTimeout = 0;
        res = -1;
    }
    return res;
}
