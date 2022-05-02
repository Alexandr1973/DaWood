#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "myserialport.h"
#include <QMessageBox>
#include "dataprocessing.h"
#include <QFileDialog>
#include <QtSql>
#include <QtSql/QSqlDatabase>
#include "pc_database.h"
#include <QProcess>
//#include <QRegularExpression>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
signals:
private slots:
    void SlotOnlineDataIsReady();
    void ReceiveCurrentDownloadDatalogs(int);
    void Send2SerialPortFromDataProcessing(char* data, unsigned char length);
    void DataProcessingIsEnd(int operation);
    void ReceiveCurrentProcessPercent(float);
    void ReceiveCurrentFirmwarePercent(float percent);
    void FirmwareUpdatedStart();
    void FirmwareUpdatedEnd();
    void DataIsUpdate ();
    void changeMode ();
    void ErrorFromDevice(int err);
    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_pushButton_ComPortsRefresh_clicked();

    void on_pushButton_ConnectionToComPort_clicked();

    void on_pushButton_Config_GetIt_clicked();

    void on_pushButton_SyncRTC_clicked();

    void on_pushButton_Formatting_clicked();


    void on_pushButton_USBSync_clicked();

    void on_pushButton_USBInDetail_clicked();

    void on_pushButton_USBDataOnline_clicked();

    void on_pushButton_USBFirmwareUpdate_clicked();

    void on_pushButton_ConfigPrevPage_clicked();

    void on_pushButton_ConfigNextPage_clicked();

    void on_pushButton_Config_SetIt_clicked();

    void on_pushButton_USBDeviceLogs_clicked();

    void on_action_triggered();

    void on_pushButton_SyncData_clicked();

    void on_action_7_triggered();

    void on_action_8_triggered();

    void on_checkBox_CalculateDataLogsByPassport_stateChanged(int arg1);

    void on_action_2_triggered();

    void on_action_4_triggered();

    void on_action_5_triggered();

    void on_lineEdit_Config_FreqReqBraid_editingFinished();

    void on_lineEdit_Config_CriticalValue_editingFinished();

    void on_lineEdit_Config_c2s_minTemp_editingFinished();

    void on_lineEdit_Config_c2s_minBattery_editingFinished();

    void on_lineEdit_Config_c2snoftry_editingFinished();

    void on_lineEdit_Config_c2s_tryperiod_editingFinished();

    void on_lineEdit_c2s_modeMinute_freq_editingFinished();

    void on_lineEdit_Config_c2s_mobileNumber_editingFinished();

    void on_action_6_triggered();

    void on_pushButton_8_clicked();

    void on_action_9_triggered();

    void on_action_3_triggered();

    void on_pushButton_SetFirmwareFilePath_clicked();

    void on_pushButton_StartUpdateFirmware_clicked();

    void on_pushButton_CheckNewFirmware_clicked();

    void on_checkBox_isOpenFolderBeforeSync_stateChanged(int arg1);
    void slotRequestStatusDevice();
    void slotRequestOnlineDevice();
    void on_pushButton_OnlineDataExport_clicked();

    void on_pushButton_OnlineStart_clicked();

    void on_comboBox_OnlineData_freqreq_activated(const QString &arg1);

    void on_pushButton_OnlineData_InklinExport_clicked();

    void on_checkBox_OnlineData_InklinAutoScroll_stateChanged(int arg1);

    void on_pushButton_OnlineDataInklinClear_clicked();

    void on_pushButton_ExtraFormatAll_clicked();

    void on_timeEdit_EveryweekMode_Time_userTimeChanged(const QTime &time);

    void on_checkBox_StartRequestSensors_stateChanged(int arg1);

private:
    bool togglePushButton_Connection2Port = false;
    bool togglePushButton_OnlineData = false;
    bool DeviceIsDetected = false;
    Ui::MainWindow *ui;
    QThread *thread;
    DataProcessing *dpro;
    MySerialPort *SerialPort;
    pc_database *db;
    tblConfig *tblcfg;
    QTimer* timerRequestStatusDevice;
    QTimer* timerRequestOnlineDataDevice;
    void batteryLevelChanged(int nValue);
    void checkPossibleNumberInQLineEdit(QLineEdit* line, const QString &arg1, int min, int max);
    void WorkingInProgressMessage(void);
    unsigned int countDataOnline = 0;
};
#endif // MAINWINDOW_H
