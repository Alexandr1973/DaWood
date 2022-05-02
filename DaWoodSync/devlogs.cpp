#include "devlogs.h"


using namespace std;

devlogs::devlogs()
{

}


int devlogs::packageProcessing(QString pathdatalogs, uint8_t* bufferData, uint32_t length, int device_id)
{
    ofstream fout;
    time_t rawtime;
    QDateTime timestamp;
    QString path2;
    QByteArray tmp;

    if (length <= 0)
    {
        return 1;
    }

    path2 = pathdatalogs + "/devlogs";

    if (QDir(path2).exists() == false)
    {
        QDir().mkdir(path2);    //Создаем папку thermo
    }
    else
        qDebug() << "Folder Devlogs already create";

    //Формируем txt файл с именем текущей даты и времени ПК
    path2 = path2 + '/' + QString::number(device_id & 0xFFFFFFF) + ".txt";




    tmp = path2.toLocal8Bit();  //Преобразуем QString -> QByteArray -> const char*

    fout.open(tmp, ios::app);
    if (!fout.is_open()) // если файл небыл открыт
    {
//        cout << "Файл device logs не может быть открыт или создан\n"; // напечатать соответствующее сообщение
        //return 1; // выполнить выход из программы
    }
    else
    {
        fout << "---------------------------------------------------------------" << endl;
        for (unsigned int i = 0; i < length / 13; i++)
        {
            emit sendCurrentNoFProcess(1);

            rawtime = (((unsigned char)bufferData[0 + (i * 13)] << 24) | ((unsigned char)bufferData[1 + (i * 13)] << 16) | ((unsigned char)bufferData[2 + (i * 13)] << 8) | (unsigned char)bufferData[3 + (i * 13)]);
            timestamp.setTimeSpec(Qt::UTC);
            timestamp.setTime_t(rawtime);

            tmp = timestamp.toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit();

            fout << tmp.constData() << ":" << setfill('0') << setw(4) << Convert8to16bits(bufferData + 4 + (i * 12)) << "\t";                  //dtime
            if (bufferData[6 + (i * 13)] == DEBUG_MESSAGE_TYPE_SYSTEM)
            {
                fout << "system" << "\t\t";
            }
            else if (bufferData[6 + (i * 13)] == DEBUG_MESSAGE_TYPE_APPLICATIONS)
            {
                fout << "app" << "\t\t";
            }
            else if (bufferData[6 + (i * 13)] == DEBUG_MESSAGE_TYPE_SOCKET_WIRELESS)
            {
                fout << "modulw" << "\t\t";
            }
            else if (bufferData[6 + (i * 13)] == DEBUG_MESSAGE_TYPE_SOCKET_SENSORS)
            {
                fout << "moduls" << "\t\t";
            }
            else if (bufferData[6 + (i * 13)] == DEBUG_MESSAGE_TYPE_SOCKET_DEBUG)
            {
                fout << "moduld" << "\t\t";
            }

            if (Convert8to32bits(bufferData + 8 + (i * 13)) == DEBUG_MESSAGE_NUMERIC_EMPTY)
            {
                fout << code_error_decrypt[bufferData[7 + (i * 13)] - 1] << "\n";
            }
            else
            {
                fout << code_error_decrypt[bufferData[7 + (i * 13)] - 1] << ":" << (int32_t)Convert8to32bits(bufferData + 8 + (i * 13)) << "\n";
            }
        }
        fout << "---------------------------------------------------------------\r\n" << endl;
        fout.close();

    }
    return 0;
}
