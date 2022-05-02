#include "piezo.h"

using namespace std;

piezo::piezo()
{

}

int piezo::packageProcessing(QString pathdatalogs, uint8_t* bufferData, uint32_t length)
{
    ofstream fout;
    time_t rawtime;
    unsigned char SerialNumberPiezo[12];
    uint16_t INTPART;
    char bMacAddrField[100];
    QDateTime timestamp;
    QString path2;
    QByteArray tmp;

    if (length <= 0)
    {
        return 1;
    }

    path2 = pathdatalogs + "/piezo";

    if (QDir(path2).exists() == false)
    {
        QDir().mkdir(path2);    //Создаем папку thermo
    }
    else
        qDebug() << "Folder Piezo already create";

    //Формируем txt файл с именем текущей даты и времени ПК
    path2 = path2 + '/' + QDateTime::currentDateTime().toString("ddMMyy_hhmmss") + ".csv";




    tmp = path2.toLocal8Bit();  //Преобразуем QString -> QByteArray -> const char*

    fout.open(tmp, ios::app);
    if (!fout.is_open()) // если файл небыл открыт
    {
        cout << "Файл piezo не может быть открыт или создан\n"; // напечатать соответствующее сообщение
        //return 1; // выполнить выход из программы
    }
    else
    {

        for (unsigned int i = 0; i < length / 18; i++)
        {
            emit sendCurrentNoFProcess(1);
            rawtime = (((unsigned char)bufferData[0 + (i * 18)] << 24) | ((unsigned char)bufferData[1 + (i * 18)] << 16) | ((unsigned char)bufferData[2 + (i * 18)] << 8) | (unsigned char)bufferData[3 + (i * 18)]);
            timestamp.setTimeSpec(Qt::UTC);
            timestamp.setTime_t(rawtime);
            tmp = timestamp.toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit();

            SerialNumberPiezo[0] = (unsigned char)bufferData[4 + (i * 18)];
            SerialNumberPiezo[1] = (unsigned char)bufferData[5 + (i * 18)];
            SerialNumberPiezo[2] = (unsigned char)bufferData[6 + (i * 18)];
            SerialNumberPiezo[3] = (unsigned char)bufferData[7 + (i * 18)];
            SerialNumberPiezo[4] = (unsigned char)bufferData[8 + (i * 18)];
            SerialNumberPiezo[5] = (unsigned char)bufferData[9 + (i * 18)];
            SerialNumberPiezo[6] = (unsigned char)bufferData[10 + (i * 18)];
            SerialNumberPiezo[7] = (unsigned char)bufferData[11 + (i * 18)];
            SerialNumberPiezo[8] = (unsigned char)bufferData[12 + (i * 18)];
            SerialNumberPiezo[9] = (unsigned char)bufferData[13 + (i * 18)];
            SerialNumberPiezo[10] = (unsigned char)bufferData[14 + (i * 18)];
            SerialNumberPiezo[11] = (unsigned char)bufferData[15 + (i * 18)];

            INTPART = (int16_t)((int16_t)((unsigned char)bufferData[16 + (i * 18)] << 8) | (unsigned char)bufferData[17 + (i * 18)]);

            sprintf(bMacAddrField, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
                SerialNumberPiezo[0],
                SerialNumberPiezo[1],
                SerialNumberPiezo[2],
                SerialNumberPiezo[3],
                SerialNumberPiezo[4],
                SerialNumberPiezo[5],
                SerialNumberPiezo[6],
                SerialNumberPiezo[7],
                SerialNumberPiezo[8],
                SerialNumberPiezo[9],
                SerialNumberPiezo[10],
                SerialNumberPiezo[11]);

            fout << tmp.constData() << ";";                  //dtime
            fout << "Piezo" << ";";             //type_s
            fout << bMacAddrField << ";";                      //mac_addr
            fout << INTPART;   //val_a
            fout << "\n";

            /*db_tblIndications_WriteIndication(connect, bDateTime, hole_id, "Piezo",
                NumberIndication, 0,
                bMacAddrField,
                ((float)((float)measurement * 10000000) / 980665),
                0);*/

        }
        fout.close();

    }
    return 0;
}
