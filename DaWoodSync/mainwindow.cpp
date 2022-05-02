#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <time.h>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    QSqlDatabase db2 = QSqlDatabase::addDatabase("QMYSQL");
//    db2.setHostName("134.0.112.143");
//    db2.setDatabaseName("systems");
//    db2.setUserName("only4test");
//    db2.setPassword("`!Q2w3e4r%T");
//    bool ok = db2.open();
//    if (ok)
//    {
//        QSqlQuery query(db2);
//        query.prepare( "SELECT * FROM status WHERE id = 1" );
//        if (!query.exec())
//        {
//            qDebug() << "Error" << query.lastError();
//        }
//        query.first();
//        qDebug() << query.value("rssi").toInt();
//        db2.close();
//        QSqlDatabase::removeDatabase("systems");
//        db2.removeDatabase("systems");
//    }

//    qDebug() << ok;
    QSettings settings("HKEY_LOCAL_MACHINE",
                            QSettings::NativeFormat);
    QString bios = settings.value("HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\BIOS\\SystemProductName\\.").toString();
    qDebug() << "Reestr" << bios;

    ui->timeEdit_StartRequestSensors->setDisabled(true);
    ui->radioButton_c2s_modeEveryMonth->setDisabled(true);
    //ui->radioButton_c2s_modeEveryweek->setDisabled(true);
    ui->pushButton_USBSync->setFocus();
    ui->groupBox_2->setDisabled(true);
    ui->groupBox_4->setDisabled(true);
    ui->pushButton_OnlineStart->setDisabled(true);
    ui->pushButton_OnlineData_InklinExport->setDisabled(true);
    ui->pushButton_StartUpdateFirmware->setDisabled(true);
    ui->radioButton_c2s_mode_Hand->setDisabled(true);
    ui->stackedWidget->setCurrentIndex(0);
    ui->stackedWidget_modeExportData->setCurrentIndex(0);
    ui->statusbar->showMessage("Подготовка приложения", 5000);
    connect(ui->radioButton_c2s_modeMinute, SIGNAL(clicked()), this, SLOT(changeMode()));
    connect(ui->radioButton_c2s_modeEveryday, SIGNAL(clicked()), this, SLOT(changeMode()));
    connect(ui->radioButton_c2s_modeEveryweek, SIGNAL(clicked()), this, SLOT(changeMode()));
    connect(ui->radioButton_c2s_modeEveryMonth, SIGNAL(clicked()), this, SLOT(changeMode()));

    SerialPort = new MySerialPort;
    SerialPort->CreateConnect();

    QList<QSerialPortInfo> com_ports = QSerialPortInfo::availablePorts();
    QSerialPortInfo port;
    ui->comboBox->clear();
    foreach(port, com_ports)
    {
        ui->comboBox->addItem(port.portName() + " (" + port.description() + ')');
    }

    db = new pc_database();
    ui->checkBox_CalculateDataLogsByPassport->setChecked(db->getParameter(3).toInt() > 0 ? true : false);
    ui->checkBox_isOpenFolderBeforeSync->setChecked(db->getParameter(5).toInt() > 0 ? true : false);
    tblcfg = new tblConfig;
    if (db->checkParameter(4))
    {
        ui->lineEdit_ViewPathFileFirmware->setText(db->getParameter(4));
    }

//    for (int i = 0; i < 200; i++)
       // ui->listWidget->addItem(QString::number(i + 1) + "Hello");
//    ui->textBrowser->insertPlainText(QString::number(i + 1) + "Hello\n");
}

void MainWindow::batteryLevelChanged(int nValue)
{
    QString myStyleSheet;
    ui->progressBar_BatteryLevel->setValue(nValue);
    myStyleSheet = ui->progressBar_BatteryLevel->styleSheet();
    myStyleSheet.append(" QProgressBar::chunk {"
    " background-color: ");

    if(nValue <= 30)
    {
        myStyleSheet.append("#ff3300;");
    }
    else if(nValue <= 80)
    {
        myStyleSheet.append("#ffcc66;");
    }
    else
    {
        myStyleSheet.append("#3add36;");
    }
    myStyleSheet.append("     width: 1px;"\
                        "     margin: 0.1px;"\
                        " }");
    ui->progressBar_BatteryLevel->setStyleSheet(myStyleSheet);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::FirmwareUpdatedStart()
{
    ui->pushButton_StartUpdateFirmware->setDisabled(true);
    ui->pushButton_SetFirmwareFilePath->setDisabled(true);
}


void MainWindow::FirmwareUpdatedEnd()
{
    ui->statusbar->showMessage("Обновление устройства завершено", 10000);
    if(ui->groupBox_4->isEnabled())
        ui->groupBox_4->setDisabled(true);
    if(ui->groupBox_2->isEnabled())
        ui->groupBox_2->setDisabled(true);
    ui->pushButton_ComPortsRefresh->setDisabled(false);
    dpro->setProcessStart(false);
    this->dpro->setStep(NOTHING_DO);

    togglePushButton_Connection2Port = false;
    ui->pushButton_ConnectionToComPort->setText("Подключиться");
    ui->comboBox->setDisabled(false);
    ui->pushButton_StartUpdateFirmware->setDisabled(false);
    ui->pushButton_SetFirmwareFilePath->setDisabled(false);
}


void MainWindow::DataIsUpdate()
{
    QByteArray Hex2Text;
    QString tmp;
    float tmpInt;
    ui->pushButton_OnlineStart->setDisabled(false);
    ui->pushButton_OnlineData_InklinExport->setDisabled(false);
    ui->groupBox_2->setDisabled(false);

    ui->groupBox_4->setDisabled(false);
    DeviceIsDetected = true;
    //Инфо о размере данных хранящиеся в устройстве
    if (dpro->dev->getDataLogsInfo().thermo_num || dpro->dev->getDataLogsInfo().piezo_num || dpro->dev->getDataLogsInfo().inklin_num || dpro->dev->getDataLogsInfo().devlogs_size)
    {
        tmp.append("Содержатся пакеты\r\n");
        ui->pushButton_SyncData->setDisabled(false);
        ui->pushButton_Formatting->setDisabled(false);
    }
    else
    {
        tmp.append("Нет данных для загрузки");
        ui->pushButton_SyncData->setText("Загрузка данных");
        ui->pushButton_SyncData->setDisabled(true);
        ui->pushButton_Formatting->setDisabled(true);
    }
    if (dpro->dev->getDataLogsInfo().thermo_num)
    {
        tmpInt = dpro->dev->getDataLogsInfo().thermo_size;
        tmp.append("-термоданные:\t" + QString::number(dpro->dev->getDataLogsInfo().thermo_num));

        if (tmpInt >= 1048576)
        {
            tmpInt /= 1048576;
            tmp.append(" (" + QString::number(floor(tmpInt*100)/100) + " Мбайт)\r\n");
        }
        else if (tmpInt >= 1024)
        {
            tmpInt /= 1024;
            tmp.append(" (" + QString::number(floor(tmpInt*100)/100) + " Кбайт)\r\n");
        }
        else
        {
            tmp.append(" (" + QString::number(tmpInt) + " байт)\r\n");
        }
    }

    if (dpro->dev->getDataLogsInfo().piezo_num)
    {
        tmp.append("-пьезоданные:\t" + QString::number(dpro->dev->getDataLogsInfo().piezo_num));
        tmp.append(" (" + QString::number(dpro->dev->getDataLogsInfo().piezo_size) + " байт)\r\n");
    }

    if (dpro->dev->getDataLogsInfo().inklin_num)
    {
        tmp.append("-инклин-е данные:\t" + QString::number(dpro->dev->getDataLogsInfo().inklin_num));
        tmp.append(" (" + QString::number(dpro->dev->getDataLogsInfo().inklin_size) + " байт)\r\n");
    }


    if (dpro->dev->getDataLogsInfo().devlogs_size)
    {
        tmpInt = dpro->dev->getDataLogsInfo().devlogs_size;
        if (tmpInt >= 1048576)  //Преобразование в МБайт
        {
            tmpInt /= 1048576;
            tmp.append("-дев_логи:\t" + QString::number(floor(tmpInt*100)/100) + " Мбайт\r\n");
        }
        else if (tmpInt >= 1024)    //Преобразование в КБайт
        {
            tmpInt /= 1024;
            tmp.append("-дев_логи:\t" + QString::number(floor(tmpInt*100)/100) + " Кбайт\r\n");
        }
        else
        {
            tmp.append("-дев_логи:\t" + QString::number(tmpInt) + " байт\r\n");
        }
    }

    ui->pushButton_SyncData->setToolTip(tmp);

    //Инфо печатной платы
    ui->lineEdit_HWRev->setText(QString(dpro->dev->getDeviceInfo().hw_rev) + " " + QString(dpro->dev->getDeviceInfo().hw_ver));
    qDebug() << QByteArray(dpro->dev->getDeviceInfo().hw_ver, 6);
    //Серийный номер
    ui->lineEdit_SerialNumber->setText(QByteArray(dpro->dev->getDeviceInfo().serial_num, 12).toHex());

    //Версия ПО
    ui->lineEdit_SWVersion->setText(dpro->dev->getDeviceInfo().sw_ver);

    //Напряжение аккумулятора
    ui->lcdNumber_BatteryVoltage->display(dpro->dev->getDeviceStatus().battery.voltage);

    //Уровень заряда аккумулятора
    batteryLevelChanged(dpro->dev->getDeviceStatus().battery.soc);

    //Объект привязанный - его УИД
    ui->lineEdit_ConnectedObject->setText(QByteArray(dpro->dev->getDeviceInfo().holeuid, 8).toHex().toUpper());

    //Разъем гирлянды - ток
    ui->lcdNumber_SocketSensorCurrent->display(dpro->dev->getDeviceStatus().senspwr.current);

    //Разъем гирлянды - напряжение
    ui->lcdNumber_SocketSensorVoltage->display(dpro->dev->getDeviceStatus().senspwr.voltage);

    //Разъем гирлянды - мощность
    ui->lcdNumber_SocketSensorPower->display(dpro->dev->getDeviceStatus().senspwr.power);

    //Температура ЦП
    ui->lcdNumber_TemperatureCPU->display(dpro->dev->getDeviceStatus().t_cpu);

    //Температура среды
    ui->lcdNumber_TemperatureOutside->display(dpro->dev->getDeviceStatus().t_ext);

    //Регистрация
    ui->lineEdit_RegistrationIntoServer->setText(QByteArray(dpro->dev->getDeviceInfo().registraion, 4).toHex());

    //Коэффициенты АЦП
    ui->lineEdit_ADCCalib_coef2volt->setText(QString::number(dpro->dev->getADCCoef().coef2voltage));
    ui->lineEdit_ADCCalib_MinBattery->setText(QString::number(dpro->dev->getADCCoef().min_battery));
    ui->lineEdit_ADCCalib_diff->setText(QString::number(dpro->dev->getADCCoef().diff));

    //Напряжение ЦП
    ui->lcdNumber_CPUVoltage->display(dpro->dev->getADCCoef().refVoltage);


    //Накопители информации
    int lentmp = (16777216 - (dpro->dev->getDataLogsInfo().inklin_size +
                                       dpro->dev->getDataLogsInfo().piezo_size +
                                       dpro->dev->getDataLogsInfo().thermo_size +
                                       dpro->dev->getDataLogsInfo().devlogs_size)) / 1024;
    ui->lcdNumber_FlashTotal->display(16777216 / 1024);
    ui->lcdNumber_FlashBusy->display(lentmp);


    ui->lcdNumber_EepromTotal->display(16384);


//    Hex2Text = Hex2Text.replace(0, 12, data.constData() + 2);
//    ui->lineEdit_Model->setText(Hex2Text);
//    Hex2Text.clear();
//    Hex2Text = Hex2Text.replace(0, 12, data.constData() + 2);
}

/**
 * Перелистывание виджета
 * */
void MainWindow::changeMode()
{
    if (ui->radioButton_c2s_modeMinute->isChecked())
        ui->stackedWidget_modeExportData->setCurrentIndex(1);
    else if (ui->radioButton_c2s_modeEveryday->isChecked())
        ui->stackedWidget_modeExportData->setCurrentIndex(2);
    else if (ui->radioButton_c2s_modeEveryweek->isChecked())
        ui->stackedWidget_modeExportData->setCurrentIndex(3);
    else if (ui->radioButton_c2s_modeEveryMonth->isChecked())
        ui->stackedWidget_modeExportData->setCurrentIndex(4);
}

void MainWindow::ErrorFromDevice(int err)
{
    QMessageBox msgBox;
    msgBox.setText("Произошла ошибка!");
    msgBox.setDetailedText("Ошибка: " + QString::number(err) + "\r\nПопробуйте перезагрузить устройство" );
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.exec();
}

void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)
{
    qDebug() <<"on_comboBox_currentTextChanged" << arg1;
}

void MainWindow::on_pushButton_ComPortsRefresh_clicked()
{
    QList<QSerialPortInfo> com_ports = QSerialPortInfo::availablePorts();
    QSerialPortInfo port;
    ui->comboBox->clear();
    foreach(port, com_ports)
    {
        ui->comboBox->addItem(port.portName() + " (" + port.description() + ')');
    }
}

void MainWindow::ReceiveCurrentProcessPercent(float percent)
{
    percent = percent > 100 ? 100 : percent;
    ui->progressBar_FullDataProcess->setFormat(QString::number(percent) + '%');
    ui->progressBar_FullDataProcess->setValue(percent);
}

void MainWindow::ReceiveCurrentFirmwarePercent(float percent)
{
    percent = percent > 100 ? 100 : percent;
    ui->progressBar_FirmwareUpdated->setFormat(QString::number(percent) + '%');
    ui->progressBar_FirmwareUpdated->setValue(percent);
}

void MainWindow::ReceiveCurrentDownloadDatalogs(int n)
{
    switch(n)
    {
    case 0:
        break;
    case 1:
        ui->statusbar->showMessage("Загрузка данных с термических гирлянд...");
        break;
    case 2:
        ui->statusbar->showMessage("Загрузка данных с пьезметрических кос...");
        break;
    case 3:
        ui->statusbar->showMessage("Загрузка данных с инклинометрических гирлянд...");
        break;
    case 4:
        ui->statusbar->showMessage("Загрузка логов устройства...");
        break;
    case 5:
        ui->statusbar->showMessage("Распаковка данных в файлы");
        break;
    case 6:
        break;
    }
}

void MainWindow::DataProcessingIsEnd(int operation)
{
    QStringList p;
//    QFont font = ui->statusbar->font();
//    font.setBold(true);

//    ui->statusbar->setFont(font);
    if (operation == 0x01)
    {
        ui->statusbar->showMessage("Загрузка данных завершена", 10000);

        ui->pushButton_SyncData->setDisabled(false);
        ui->pushButton_ConnectionToComPort->setDisabled(false);

        if (ui->checkBox_isOpenFolderBeforeSync->isChecked() == true)
        {
            p.append(db->getParameter(1).replace("/","\\"));
            qDebug() << p.first();
            QProcess::startDetached("explorer", p);
        }
        ui->pushButton_SyncData->setText("Данные загружены. Загрузить еще раз");
    }
    else if (operation == 0x02)     //Синхронизация RTC
    {
//        ui->pushButton_SyncRTC->setText("Повторить синхронизацию");
        ui->statusbar->showMessage("Синхронизация времени завершена", 10000);
    }
    else if (operation == 0x03)     //Форматирование флеш памяти
    {
        ui->statusbar->showMessage("Форматирование завершено", 10000);
    }
    else if (operation == 0x04)     //Форматирование флеш памяти
    {
        ui->pushButton_SyncData->setDisabled(false);
        ui->pushButton_SyncRTC->setDisabled(false);
        ui->pushButton_Formatting->setDisabled(false);
        ui->pushButton_Config_SetIt->setDisabled(false);
        ui->statusbar->showMessage("Настройки устройством приняты", 5000);
    }
}

void MainWindow::on_pushButton_ConnectionToComPort_clicked()
{
    if (!togglePushButton_Connection2Port)
    {
        QString tmp = ui->comboBox->currentText();

        if (SerialPort->openSerialPort(tmp.left(tmp.indexOf(' '))) == 1)
        {
            thread= new QThread;
            dpro = new DataProcessing(3000, SerialPort);

            dpro->moveToThread(thread);
            connect(thread, SIGNAL(started()), dpro, SLOT(proccessing()));
            connect(dpro, SIGNAL(DataIsReady()), this, SLOT(DataIsUpdate()));
            connect(dpro, SIGNAL(ErrorOccured(int)), this, SLOT(ErrorFromDevice(int)));
            connect(dpro, SIGNAL(SendCurrentPercentProcess(float)), this, SLOT(ReceiveCurrentProcessPercent(float)));
            connect(dpro, SIGNAL(SendCurrentFirmwarePercent(float)), this, SLOT(ReceiveCurrentFirmwarePercent(float)));
            connect(dpro, SIGNAL(SendFirmwareUpdatedEnd()), this, SLOT(FirmwareUpdatedEnd()));
            connect(dpro, SIGNAL(SendFirmwareUpdatedStart()), this, SLOT(FirmwareUpdatedStart()));
            connect(dpro, SIGNAL(isComplete(int)), this, SLOT(DataProcessingIsEnd(int)));
            connect(dpro, SIGNAL(SendCurrentDownloadDatalogs(int)), this, SLOT(ReceiveCurrentDownloadDatalogs(int)));
            connect(dpro, SIGNAL(Send2SerialPort(char*, unsigned char)), this, SLOT(Send2SerialPortFromDataProcessing(char*, unsigned char)));
            connect(dpro, &DataProcessing::finished, thread, &QThread::terminate);
            connect(dpro, SIGNAL(OnlineDataIsReady()), this, SLOT(SlotOnlineDataIsReady()));
//            timerRequestStatusDevice = new QTimer();
//            connect(timerRequestStatusDevice, SIGNAL(timeout()), this, SLOT(slotRequestStatusDevice()));
            this->dpro->setStep(SEARCH_MY_DEVICE);
//            timerRequestStatusDevice->start(5000);
            thread->start();

            qDebug() <<"DataProcessing create";
            ui->pushButton_ConnectionToComPort->setText("Отключиться");
            ui->comboBox->setDisabled(true);
            ui->pushButton_ComPortsRefresh->setDisabled(true);
            togglePushButton_Connection2Port = true;
            ui->pushButton_StartUpdateFirmware->setDisabled(false);
        }
    }
    else
    {
//        timerRequestStatusDevice->stop();
//        delete[] timerRequestStatusDevice;
        ui->statusbar->showMessage("Отключаемся, подождите...", 1500);
        if(ui->groupBox_4->isEnabled())
            ui->groupBox_4->setDisabled(true);
        if(ui->groupBox_2->isEnabled())
            ui->groupBox_2->setDisabled(true);
        ui->pushButton_ComPortsRefresh->setDisabled(false);
        this->dpro->setStep(NOTHING_DO);
        dpro->setProcessStart(false);
        togglePushButton_Connection2Port = false;
        ui->pushButton_ConnectionToComPort->setText("Подключиться");
        ui->comboBox->setDisabled(false);
        ui->pushButton_StartUpdateFirmware->setDisabled(true);
//        SerialPort->closeSerialPort();

//        if (thread != NULL)
//            delete[]thread;
//        if (dpro != NULL)
//            delete[]dpro;
    }

}

void MainWindow::slotRequestStatusDevice()
{
    this->dpro->setStep(SEARCH_MY_DEVICE);
}

void MainWindow::on_pushButton_Config_GetIt_clicked()
{
    /*****************Настройки********************/
    //Режим работы устройства
    if(dpro->dev->getTblConfig().operating_mode == tblConfig::Remote)
        ui->radioButton_c2s_mode_Remote->click();
    else if (dpro->dev->getTblConfig().operating_mode == tblConfig::Cumulative)
        ui->radioButton_c2s_mode_Cumulative->click();
    else if (dpro->dev->getTblConfig().operating_mode == tblConfig::Hand)
        ui->radioButton_c2s_mode_Hand->click();

    //Частота опроса гирлянды
    ui->lineEdit_Config_FreqReqBraid->setText(QString::number(dpro->dev->getTblConfig().freq_req));

    //Время начала опроса гирлянды
    ui->checkBox_StartRequestSensors->setChecked(dpro->dev->getTblConfig().isStartTimeReq);
    ui->timeEdit_StartRequestSensors->setTime(dpro->dev->getTblConfig().StartTimeReq);

    //Критическое значение
    ui->lineEdit_Config_CriticalValue->setText(QString::number(dpro->dev->getTblConfig().critical_value));

    //Режим экспорта данных
    if(dpro->dev->getTblConfig().c2s.mode == 0x01)
    {
        ui->radioButton_c2s_modeEveryday->click();
    }
    else if (dpro->dev->getTblConfig().c2s.mode == 0x00)
    {
        ui->radioButton_c2s_modeMinute->click();
    }

    //Время подключения
    uint x = dpro->dev->getTblConfig().c2s.date_time;
    ui->timeEdit_c2s_modeEveryday_time->setTime(QDateTime::fromTime_t(x).time());

    //Частота подключения к серверу
    ui->lineEdit_c2s_modeMinute_freq->setText(QString::number(dpro->dev->getTblConfig().c2s.freq_export));

    //c2s Минимальная температура
    ui->lineEdit_Config_c2s_minTemp->setText(QString::number(dpro->dev->getTblConfig().c2s.min_temperature));

    //c2s Минимальный уровень заряда
    ui->lineEdit_Config_c2s_minBattery->setText(QString::number(dpro->dev->getTblConfig().c2s.min_battery));

    //c2s Кол-во попыток
    ui->lineEdit_Config_c2snoftry->setText(QString::number(dpro->dev->getTblConfig().c2s.nb_of_try));

    //c2s Период между попытками
    ui->lineEdit_Config_c2s_tryperiod->setText(QString::number(dpro->dev->getTblConfig().c2s.try_period));

    //c2s Номер телефона смс оповещения
    ui->lineEdit_Config_c2s_mobileNumber->setText(QByteArray(dpro->dev->getTblConfig().mobileNum1, 12));

    //Логи в устройстве
    ui->checkBox_DevLogs->setChecked(dpro->dev->getTblConfig().flag_syncdlogs);

    //IP Адрес сервера
    ui->lineEdit_c2s_ipaddr->setText(dpro->dev->getTblConfig().c2s.ipaddr);

    //Порт сервера
    ui->lineEdit_c2s_ipport->setText(dpro->dev->getTblConfig().c2s.ipport);
}

void MainWindow::on_pushButton_SyncRTC_clicked()
{
    dpro->setStep(SYNCHRONIZATION_DATETIME);
}

void MainWindow::Send2SerialPortFromDataProcessing(char* data, unsigned char length)
{
    SerialPort->sendData(data, length);
}

void MainWindow::on_pushButton_Formatting_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("Вы действительно хотите очистить память?");
    msgBox.setInformativeText("Очистка произведет удаление всех накопленных данных с сенсорных гирлянд в том числе логи устройства.");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.QDialog::setWindowTitle("Предупреждение");
    msgBox.setDefaultButton(QMessageBox::Ok);
    int res = msgBox.exec();
    if (res == QMessageBox::Ok) //нажата кнопка Ok
    {
        dpro->setStep(REQUEST_FORMAT_FLASH);
        ui->statusbar->showMessage("Форматируется флеш память, подождите...");
    }
}

void MainWindow::on_pushButton_USBSync_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_pushButton_USBInDetail_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_USBDataOnline_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);

}

void MainWindow::slotRequestOnlineDevice()
{
    this->dpro->setStep(REQUEST_ONLINE_DATA);
}

void MainWindow::on_pushButton_USBFirmwareUpdate_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::on_pushButton_ConfigPrevPage_clicked()
{
    int ci =ui->stackedWidget_Config->currentIndex();
    ui->stackedWidget_Config->setCurrentIndex(--ci < 0 ? 2 : ci);
}

void MainWindow::on_pushButton_ConfigNextPage_clicked()
{
    int ci =ui->stackedWidget_Config->currentIndex();
    ui->stackedWidget_Config->setCurrentIndex(++ci > 2 ? 0 : ci);
}

void MainWindow::on_pushButton_Config_SetIt_clicked()
{
    QString str;
    QMessageBox msgBox;
    QDateTime tm;
    QDate dayweek;
    int res;
    bool isEmptyParameter = false;

    isEmptyParameter |= ui->lineEdit_Config_FreqReqBraid->text().isEmpty();
    isEmptyParameter |= ui->lineEdit_Config_CriticalValue->text().isEmpty();
    isEmptyParameter |= ui->lineEdit_Config_c2s_minTemp->text().isEmpty();
    isEmptyParameter |= ui->lineEdit_Config_c2s_minBattery->text().isEmpty();
    isEmptyParameter |= ui->lineEdit_Config_c2snoftry->text().isEmpty();
    isEmptyParameter |= ui->lineEdit_Config_c2s_tryperiod->text().isEmpty();
    isEmptyParameter |= ui->lineEdit_Config_c2s_mobileNumber->text().isEmpty();
    isEmptyParameter |= ui->lineEdit_c2s_modeMinute_freq->text().isEmpty();
    isEmptyParameter |= (ui->radioButton_c2s_modeMinute->isChecked() == false) && (ui->radioButton_c2s_modeEveryday->isChecked() == false);
    isEmptyParameter |= ui->lineEdit_c2s_ipaddr->text().isEmpty();
    isEmptyParameter |= ui->lineEdit_c2s_ipport->text().isEmpty();

    if (isEmptyParameter == true)
    {
        msgBox.setText("Необходимо заполнить все настройки");
        msgBox.setInformativeText(str);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.QDialog::setWindowTitle("Отправка настройки");
        msgBox.setDefaultButton(QMessageBox::Ok);
        res = msgBox.exec();
        return;
    }
    //Режим работы устройства
    str.append(ui->groupBox_5->title() + ": \t\t");

    if(ui->radioButton_c2s_mode_Remote->isChecked())
        str.append(ui->radioButton_c2s_mode_Remote->text());
    else if(ui->radioButton_c2s_mode_Hand->isChecked())
        str.append(ui->radioButton_c2s_mode_Hand->text());
    else if(ui->radioButton_c2s_mode_Cumulative->isChecked())
        str.append(ui->radioButton_c2s_mode_Cumulative->text());

    //Логи в устройстве
    str.append("\n" + ui->checkBox_DevLogs->text() + ": \t\t\t" + (ui->checkBox_DevLogs->isChecked() ? "Включены" : "Отключены"));

    //Частота опроса гирлянды
    str.append("\n" + ui->label_15->text() + ": \t\t" + ui->lineEdit_Config_FreqReqBraid->text() + ui->label_28->text());

    //Критическое значение
    str.append("\n" + ui->label_29->text() + ": \t\t\t" + ui->lineEdit_Config_CriticalValue->text() + ui->label_30->text());

    /*****Начало Настройки подключения на сервер*****/
    //Мин температура
    str.append("\n" + ui->label_76->text() + ": \t\t\t" + ui->lineEdit_Config_c2s_minTemp->text() + ui->label_90->text());
    //Мин уровень заряда
    str.append("\n" + ui->label_89->text() + ": \t\t" + ui->lineEdit_Config_c2s_minBattery->text() + ui->label_91->text());
    //Кол-во попыток
    str.append("\n" + ui->label_86->text() + ": \t\t\t" + ui->lineEdit_Config_c2snoftry->text());
    //Интервал
    str.append("\n" + ui->label_87->text() + ": \t\t\t\t" + ui->lineEdit_Config_c2s_tryperiod->text() + ui->label_92->text());
    //СМС номер
    str.append("\n" + ui->label_88->text() + ": \t\t\t\t" + ui->lineEdit_Config_c2s_mobileNumber->text());
    //Режим работы соедниения на сервер
    str.append("\n" + ui->groupBox_6->title() + ": \t");
    if (ui->radioButton_c2s_modeMinute->isChecked())
    {
        str.append(ui->radioButton_c2s_modeMinute->text());
        str.append("\n" + ui->label_16->text() + ": \t\t" + ui->lineEdit_c2s_modeMinute_freq->text() + ui->label_93->text());
    }
    if (ui->radioButton_c2s_modeEveryday->isChecked())
    {
        str.append(ui->radioButton_c2s_modeEveryday->text());
        str.append("\n" + ui->label_18->text() + "в \t\t\t" + ui->timeEdit_c2s_modeEveryday_time->text());
    }
    if (ui->radioButton_c2s_modeEveryweek->isChecked())
    {
        str.append(ui->radioButton_c2s_modeEveryweek->text());
    }
    if (ui->radioButton_c2s_modeEveryMonth->isChecked())
    {
        str.append(ui->radioButton_c2s_modeEveryMonth->text());
    }
    //IP-адрес сервера
    str.append("\n" + ui->label_40->text() + ": \t\t\t\t" + ui->lineEdit_c2s_ipaddr->text());
    //Порт сервера
    str.append("\n" + ui->label_41->text() + ": \t\t\t\t\t" + ui->lineEdit_c2s_ipport->text());

    /*****Конец Настройки подключения на сервер*****/

    dpro->setStep(NOTHING_DO);
    msgBox.setText("Проверьте пожалуйста Ваши настройки перед отправкой их на устройство");
    msgBox.setInformativeText(str);
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.QDialog::setWindowTitle("Отправка настройки");
    msgBox.setDefaultButton(QMessageBox::Ok);
    res = msgBox.exec();

    if (res == QMessageBox::Ok) //нажата кнопка Ok
    {
//        qDebug() << QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss") << "CRC is valid";
        tm = QDateTime::currentDateTime();

        tm.setTime(ui->timeEdit_c2s_modeEveryday_time->time());
//        qDebug() << tm.toTime_t();
        if (ui->radioButton_c2s_mode_Remote->isChecked())
            tblcfg->setOperationMode(tblConfig::Remote);
        else if (ui->radioButton_c2s_mode_Cumulative->isChecked())
            tblcfg->setOperationMode(tblConfig::Cumulative);
        else if (ui->radioButton_c2s_mode_Hand->isChecked())
            tblcfg->setOperationMode(tblConfig::Hand);
        tblcfg->setDeviceLogsEnabled(ui->checkBox_DevLogs->isChecked());
        tblcfg->setFreqReqSenors(ui->lineEdit_Config_FreqReqBraid->text().toUInt());
        tblcfg->setCriticalValue(ui->lineEdit_Config_CriticalValue->text().toUInt());
        tblcfg->setC2SMinimumTemperature(ui->lineEdit_Config_c2s_minTemp->text().toInt());
        tblcfg->setC2SMinimumBattery(ui->lineEdit_Config_c2s_minBattery->text().toUInt());
        tblcfg->setC2SNumberOfTry(ui->lineEdit_Config_c2snoftry->text().toUInt());
        tblcfg->setC2STryPeriod(ui->lineEdit_Config_c2s_tryperiod->text().toUInt());
        tblcfg->setC2SMinutesModeFreq(ui->lineEdit_c2s_modeMinute_freq->text().toUInt());
        tblcfg->setC2SDateTime(tm.toTime_t());
        if (ui->radioButton_c2s_modeMinute->isChecked())
            tblcfg->setC2Smode(tblConfig::Minutes);
        else if (ui->radioButton_c2s_modeEveryday->isChecked())
            tblcfg->setC2Smode(tblConfig::Everyday);
        else if (ui->radioButton_c2s_modeEveryweek->isChecked())
            tblcfg->setC2Smode(tblConfig::Everyweek);
        else if (ui->radioButton_c2s_modeEveryMonth->isChecked())
            tblcfg->setC2Smode(tblConfig::Everymonth);
        QByteArray tmp = ui->lineEdit_Config_c2s_mobileNumber->text().toLocal8Bit();
        tblcfg->setMobileTelephoneNumber1(tmp.data());

        tblcfg->setIPAddrServer(ui->lineEdit_c2s_ipaddr->text());
        tblcfg->setPortServer(ui->lineEdit_c2s_ipport->text());
        tblcfg->setIsStartTimeReq(ui->checkBox_StartRequestSensors->isChecked());
        tblcfg->setStartTimeReq(ui->timeEdit_StartRequestSensors->time());
        dpro->giveMeTblConfig(tblcfg->getTblConfig());
        dpro->setStep(SEND_CONFIG_FILE);    //Отправка пакета
        ui->statusbar->showMessage("Настройки отправляются, подождите...");
        ui->pushButton_SyncData->setDisabled(true);
        ui->pushButton_SyncRTC->setDisabled(true);
        ui->pushButton_Formatting->setDisabled(true);
        ui->pushButton_Config_SetIt->setDisabled(true);

    }
    else
    {
        dpro->setStep(SEARCH_MY_DEVICE);
    }

}

void MainWindow::on_pushButton_USBDeviceLogs_clicked()
{
    WorkingInProgressMessage();
}

void MainWindow::on_action_triggered()
{
    QMessageBox msgBox;
    QString tmp = //"<h1 style='color: #5e9ca0; text-align: center;'><img src=':/image/res/logo_hres.png' /></h1>"
   //         "<img style='float: left;' <img src=':/icons/res/VnimsDataLogger2.ico' alt=""/>"
    "<strong><span style='text-align: left;'>DaWood Sync</span></strong>"
    "<p style='text-align: left;'>Версия v2.0.0b</p>"
    "<p style='text-align: left;'>Написано на Qt 5.15.2 (64bit)</p>"
    "<p style='text-align: left;'>&copy; 2021 ВНИМС ИМЗ СО РАН. Все права защищены</p>";
    msgBox.setTextFormat(Qt::TextFormat::MarkdownText);
    msgBox.setInformativeText(tmp);
    msgBox.exec();;
}


void MainWindow::on_pushButton_SyncData_clicked()
{
    QMessageBox msgBox;
    int res;
    QStringList tmp;

    if (db->checkParameter(1) == false) //Папка с данными
    {

        msgBox.setText("До начала загрузки данных нужно выбрать папку куда их сохранять");
//        msgBox.setInformativeText("Этот путь будет по умолчанию равен папке где располагается файл с паспортом. Вы можете изменить путь с паспортом в Разделе Файл->Изменить->Для паспорта");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.QDialog::setWindowTitle("Информация");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setDefaultButton(QMessageBox::Ok);
        res = msgBox.exec();
        if (res == QMessageBox::Cancel) //нажата кнопка Ok
            return;

        ui->action_7->activate(QAction::Trigger);
    }

    if (db->checkParameter(2) == false)     //Папка с паспортом
    {
        if (ui->checkBox_CalculateDataLogsByPassport->isChecked() == true)
        {
            msgBox.setText("Выберите файл паспорта");
            msgBox.setInformativeText("Вами не выбран файл паспорта и у Вас установлена галочка \"Производить расчет по паспорту\".\r\nВыбирите файл паспорта выданный изготовителем гирлянды из сенсоров");
            msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            msgBox.QDialog::setWindowTitle("Информация");
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setDefaultButton(QMessageBox::Ok);
            res = msgBox.exec();
            if (res == QMessageBox::Cancel) //нажата кнопка Ok
                return;
            ui->action_8->activate(QAction::Trigger);
        }
    }
    if ((db->checkParameter(1) == true) && (db->checkParameter(2) == true)) //Папка с данными
    {
        tmp.insert(0, db->getParameter(1)); //Папка хранения даталогов
        tmp.insert(1, db->getParameter(2)); //Файл паспорт

        qDebug() << "Папка хранения даталогов: " << tmp.at(0);
        qDebug() << "Файл паспорт: " << tmp.at(1);

        dpro->setPathForDatalogs(tmp);
        this->dpro->setStep(SYNC_INIT);
        if(ui->groupBox_4->isEnabled())
            ui->groupBox_4->setDisabled(true);
        if(ui->groupBox_2->isEnabled())
            ui->groupBox_2->setDisabled(true);
        ui->pushButton_ConnectionToComPort->setDisabled(true);
        ui->statusbar->showMessage("Не отключайте устройство от ПК. Идет загрузка данных...");
    }
}

//Выбор папки с данными
void MainWindow::on_action_7_triggered()
{
    QTemporaryDir dir;
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::DirectoryOnly);
    dialog.setOption(QFileDialog::ShowDirsOnly, false);
    if (dialog.exec() == QDialog::Accepted)
    {
        db->writeParameter(1, dialog.directory().path());
    }
}

//Выбор файла паспорта
void MainWindow::on_action_8_triggered()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setOption(QFileDialog::ReadOnly, false);
    if (dialog.exec() == QDialog::Accepted)
    {
        db->writeParameter(2, dialog.selectedFiles().takeFirst());
    }
}

void MainWindow::on_checkBox_CalculateDataLogsByPassport_stateChanged(int arg1)
{
    db->writeParameter(3, QString::number(arg1));
}

void MainWindow::on_action_2_triggered()
{
    QCoreApplication::quit();
}

void MainWindow::on_action_4_triggered()
{
    QStringList p;
    QMessageBox msgBox;
    int res;
    if (db->checkParameter(1) == true) //Папка с данными
    {
        p.append(db->getParameter(1).replace("/","\\"));
        qDebug() << p.first();
        QProcess::startDetached("explorer", p);
    }
    else
    {
        msgBox.setText("Вы пытаетесь открыть папку которую не назначили");
        msgBox.setInformativeText("Выбирите папку где будут хранится выгруженные данные");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.QDialog::setWindowTitle("Информация");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setDefaultButton(QMessageBox::Ok);
        res = msgBox.exec();
        if (res == QMessageBox::Cancel) //нажата кнопка Ok
            return;

        ui->action_7->activate(QAction::Trigger);
    }
}

void MainWindow::on_action_5_triggered()
{
    QStringList p;
    QMessageBox msgBox;
    int res;
    if (db->checkParameter(2) == true) //Папка с данными
    {
        p.append(db->getParameter(2).replace("/","\\"));
        qDebug() << p.first();
        QProcess::startDetached("explorer", p);
    }
    else
    {
        msgBox.setText("Вы пытаетесь открыть папку которую не назначили");
        msgBox.setInformativeText("Выбирите файл паспорта выданный изготовителем гирлянды из сенсоров");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.QDialog::setWindowTitle("Информация");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setDefaultButton(QMessageBox::Ok);
        res = msgBox.exec();
        if (res == QMessageBox::Cancel) //нажата кнопка Ok
            return;

        ui->action_8->activate(QAction::Trigger);
    }
}

void MainWindow::checkPossibleNumberInQLineEdit(QLineEdit* line, const QString &arg1, int min, int max)
{
    int tmp;
    QString tmp2 = arg1;


//    if (tmp2.isEmpty() == false)
    {
        tmp = tmp2.toInt();
        if (tmp < min)
            tmp = min;
        else if (tmp > max)
            tmp = max;

        line->setText(QString::number(tmp));
    }
}

void MainWindow::on_lineEdit_Config_FreqReqBraid_editingFinished()
{
    checkPossibleNumberInQLineEdit(ui->lineEdit_Config_FreqReqBraid, ui->lineEdit_Config_FreqReqBraid->text(), 1, 1439);

    qDebug() << "Freq req: " << tblcfg->getTblConfig().freq_req;
}

void MainWindow::on_lineEdit_Config_CriticalValue_editingFinished()
{
    checkPossibleNumberInQLineEdit(ui->lineEdit_Config_CriticalValue, ui->lineEdit_Config_CriticalValue->text(), 0, 100);

}

void MainWindow::on_lineEdit_Config_c2s_minTemp_editingFinished()
{
    checkPossibleNumberInQLineEdit(ui->lineEdit_Config_c2s_minTemp, ui->lineEdit_Config_c2s_minTemp->text(), -70, 0);

    qDebug() << "c2s.min_temperature: " << tblcfg->getTblConfig().c2s.min_temperature;
}

void MainWindow::on_lineEdit_Config_c2s_minBattery_editingFinished()
{
    checkPossibleNumberInQLineEdit(ui->lineEdit_Config_c2s_minBattery, ui->lineEdit_Config_c2s_minBattery->text(), 16, 50);

}

void MainWindow::on_lineEdit_Config_c2snoftry_editingFinished()
{
    checkPossibleNumberInQLineEdit(ui->lineEdit_Config_c2snoftry, ui->lineEdit_Config_c2snoftry->text(), 1, 10);

}

void MainWindow::on_lineEdit_Config_c2s_tryperiod_editingFinished()
{
    checkPossibleNumberInQLineEdit(ui->lineEdit_Config_c2s_tryperiod, ui->lineEdit_Config_c2s_tryperiod->text(), 1, 300);

}

void MainWindow::on_lineEdit_c2s_modeMinute_freq_editingFinished()
{
    checkPossibleNumberInQLineEdit(ui->lineEdit_c2s_modeMinute_freq, ui->lineEdit_c2s_modeMinute_freq->text(), 1, 1439);

}

void MainWindow::on_lineEdit_Config_c2s_mobileNumber_editingFinished()  //9142862949
{
    QString tmp2 = ui->lineEdit_Config_c2s_mobileNumber->text();

    if ((tmp2.length() > 12) || (tmp2.length() < 12))
    {
        ui->lineEdit_Config_c2s_mobileNumber->setText(QByteArray(dpro->dev->getTblConfig().mobileNum1, 12));
//        qDebug() << tmp2;
//        tmp2.remove(10,tmp2.length() - 10);

    }
    qDebug() << "length more 10";
    qDebug() << tmp2;

}

void MainWindow::on_action_6_triggered()
{
    QString tmp;
    tmp =   "<h1 style='color: #5e9ca0; text-align: center;'><img src=':/image/res/logo_text.png' alt="" /></h1>"
            "<h1 style='color: #5e9ca0; text-align: center;'><img src=':/image/res/office_house.png' alt="" /></h1>"
           // "<h1 style='text-align: center;'>""<span>Вилюйская научно-исследовательская мерзлотная станция</span></h1></br>"
            "<p style='text-align: center;'>Института мерзлотоведения Сибирского отделения Академии наук Российской Федерации организована в связи с необходимостью проведения геокриологических исследований в районах месторождений алмазов в Якутской АССР, а также геотеплофизических исследований в районе строительства Вилюйской ГЭС (Постановление Президиума Сибирского отделения АН СССР № 232 от 9 апреля 1965 года)</p>";
    QMessageBox msgBox;
    msgBox.setTextFormat(Qt::TextFormat::MarkdownText);
    msgBox.setInformativeText(tmp);
//    msgBox.setIcon(QMessageBox::NoIcon);
    msgBox.exec();
}

void MainWindow::on_pushButton_8_clicked()
{
    WorkingInProgressMessage();
}


void MainWindow::WorkingInProgressMessage(void)
{
    QString WorkInProgress;

    WorkInProgress =   "<div style='text-align: center'><img src=':/image/res/working_wait.png'/></div><br />"
          //  "<img src=':/image/res/working_wait.png' align: 'left'>"
            "<p  style: text-align:'right' >Ведутся работы над функциями программы. Следите за новостями от ВНИМС ИМЗ СО РАН</p>";

    QMessageBox msgBox;
    msgBox.setTextFormat(Qt::TextFormat::MarkdownText);
    msgBox.setInformativeText(WorkInProgress);
//    msgBox.setIcon(QMessageBox::NoIcon);
    msgBox.exec();
}

void MainWindow::on_action_9_triggered()
{
    WorkingInProgressMessage();
}

void MainWindow::on_action_3_triggered()
{
    WorkingInProgressMessage();
}

void MainWindow::on_pushButton_SetFirmwareFilePath_clicked()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::ExistingFile);
    if (db->checkParameter(4))
        dialog.setDirectory(db->getParameter(4));
    dialog.setOption(QFileDialog::ReadOnly, false);
    if (dialog.exec() == QDialog::Accepted)
    {
        db->writeParameter(4, dialog.selectedFiles().takeFirst());
        ui->lineEdit_ViewPathFileFirmware->setText(db->getParameter(4));
    }
}

void MainWindow::on_pushButton_StartUpdateFirmware_clicked()
{
    QString tmp = db->getParameter(4);
    qDebug() << "Файл прошивки: " << tmp;
    dpro->setPathFirmwareFile(tmp);
    this->dpro->setStep(REQUEST_FIRMWARE_UPDATE);
}

void MainWindow::on_pushButton_CheckNewFirmware_clicked()
{
    WorkingInProgressMessage();
}

void MainWindow::on_checkBox_isOpenFolderBeforeSync_stateChanged(int arg1)
{
    db->writeParameter(5, QString::number(arg1));
}

void MainWindow::on_pushButton_OnlineDataExport_clicked()
{
    this->dpro->setStep(REQUEST_ONLINE_DATA);
}

void MainWindow::on_pushButton_OnlineStart_clicked()
{
    if (!togglePushButton_OnlineData)
    {
        togglePushButton_OnlineData = true;
        this->dpro->setStep(SEND_ONLINE_DATA_START);

        timerRequestOnlineDataDevice = new QTimer();
        connect(timerRequestOnlineDataDevice, SIGNAL(timeout()), this, SLOT(slotRequestOnlineDevice()));
        timerRequestOnlineDataDevice->start(ui->comboBox_OnlineData_freqreq->currentText().toUInt() * 1000);
        ui->pushButton_OnlineStart->setText("Остановка");
    }
    else
    {
        togglePushButton_OnlineData = false;
        this->dpro->setStep(SEND_ONLINE_DATA_STOP);
        timerRequestOnlineDataDevice->stop();
        ui->pushButton_OnlineStart->setText("Запуск");
    }

}


void MainWindow::on_comboBox_OnlineData_freqreq_activated(const QString &arg1)
{
    if (togglePushButton_OnlineData)
        timerRequestOnlineDataDevice->start(arg1.toUInt() * 1000);
//    qDebug() << arg1.toUInt();
}

void MainWindow::SlotOnlineDataIsReady()
{
    QString stringOnlineData;
    qDebug() << "SlotOnlineDataIsReady";

    for (int i = 0; i < dpro->inklinOnlineLength; i++)
    {
        stringOnlineData.append(QString::number(i + 1) + '\t' + QByteArray((char*)dpro->inklinOnline[i].serialnum, 8).toHex().toUpper() + "\r\n");

    }
    ui->textBrowser_InklinComposition->setText(stringOnlineData);

    stringOnlineData = QDateTime::currentDateTime().toString("hh:mm:ss") + "\tx\t" + "y\t" + "z\t" +  "\r\n";
    for (int i = 0; i < dpro->inklinOnlineLength; i++)
    {
        stringOnlineData += QString::number(i + 1) + '\t' +
                QString::number(dpro->inklinOnline[i].axisX) + '\t' +
                QString::number(dpro->inklinOnline[i].axisY) + '\t' +
                QString::number(dpro->inklinOnline[i].axisZ) + "\r\n";

    }
    ui->textBrowser_OnlineDataInklin->insertPlainText(stringOnlineData);
    ui->textBrowser_OnlineDataInklin->insertPlainText("\r\n");
    if (ui->checkBox_OnlineData_InklinAutoScroll->isChecked())
        ui->textBrowser_OnlineDataInklin->moveCursor(QTextCursor::End);
    countDataOnline++;
}

void MainWindow::on_pushButton_OnlineData_InklinExport_clicked()
{
    QTemporaryDir dir;
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::DirectoryOnly);
    dialog.setOption(QFileDialog::ShowDirsOnly, false);
    if (dialog.exec() == QDialog::Accepted)
    {
        QFile file(dialog.directory().path() + "/Inklin_" + QDateTime::currentDateTime().toString("ddMMyy_hhmmss") + ".csv");
        if(file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
          // We're going to streaming text to the file
          QTextStream stream(&file);

          stream << ui->textBrowser_OnlineDataInklin->toPlainText();

          file.close();
          qDebug() << "Writing finished";
        }
        ui->textBrowser_OnlineDataInklin->clear();
    }


}

void MainWindow::on_checkBox_OnlineData_InklinAutoScroll_stateChanged(int arg1)
{
    if (arg1)
        ui->textBrowser_OnlineDataInklin->moveCursor(QTextCursor::End);
}

void MainWindow::on_pushButton_OnlineDataInklinClear_clicked()
{
    ui->textBrowser_OnlineDataInklin->clear();
}

void MainWindow::on_pushButton_ExtraFormatAll_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("Сервисное очищение");
    msgBox.setInformativeText("Очистка произведет удаление всех накопленных данных с сенсорных гирлянд в том числе логи устройства.");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.QDialog::setWindowTitle("Предупреждение");
    msgBox.setDefaultButton(QMessageBox::Ok);
    int res = msgBox.exec();
    if (res == QMessageBox::Ok) //нажата кнопка Ok
    {
        dpro->setStep(REQUEST_FORMAT_FLASH_SERVICE);
        ui->statusbar->showMessage("Форматируется флеш память, подождите...");
    }
}

void MainWindow::on_timeEdit_EveryweekMode_Time_userTimeChanged(const QTime &time)
{
//    QDate d;
//    QDateTime dayweek = QDateTime::currentDateTime();
//    dayweek = dayweek.addDays(ui->comboBox_EveryweekMode_Week->currentIndex() + 1);
//    qDebug() << dayweek.date().dayOfWeek();
//    dayweek.setTime(time);
//    qDebug() << "Hour: " << ui->timeEdit_EveryweekMode_Time->time().hour();
//    qDebug() << "Minute: " << ui->timeEdit_EveryweekMode_Time->time().minute();


}

void MainWindow::on_checkBox_StartRequestSensors_stateChanged(int arg1)
{
    ui->timeEdit_StartRequestSensors->setDisabled(!arg1);
    if (arg1)
    {
        QMessageBox msgBox;
        msgBox.setInformativeText("Не забудьте синхронизировать время");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.QDialog::setWindowTitle("Предупреждение");
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
    }

}
