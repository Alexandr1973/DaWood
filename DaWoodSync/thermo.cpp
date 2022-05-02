#include "thermo.h"

using namespace std;

thermo::thermo()
{

}

int thermo::packageProcessing(QString pathdatalogs, QString pathpassport, uint8_t* bufferData, uint32_t length)
{
    ofstream fout;
    int *ArrayForSort;
    pckt_thermobraid *pThermoPck;
    time_t rawtime;
    unsigned int TmpNumberOfDevces = 0;
    unsigned int LastSizeValue = 0;
    char  bMacAddrField[100];
    QString path2;
    QByteArray tmp;

    if (length <= 0)
    {
        return 1;
    }

    path2 = pathdatalogs + "/thermo";

    if (QDir(path2).exists() == false)
    {
        QDir().mkdir(path2);    //Создаем папку thermo
    }
    else
        qDebug() << "Folder Thermo already create";

    //Формируем txt файл с именем текущей даты и времени ПК
    path2 = path2 + '/' + QDateTime::currentDateTime().toString("ddMMyy_hhmmss") + ".csv";




    tmp = path2.toLocal8Bit();  //Преобразуем QString -> QByteArray -> const char*


    fout.open(tmp.constData(), ios::app);
    if (!fout.is_open()) // если файл небыл открыт
    {
        qDebug() << "Файл thermo не может быть открыт или создан\n"; // напечатать соответствующее сообщение
        return -1; // выполнить выход из программы
    }
    else
    {
        qDebug() << "Файл thermo открыт или создан\n";


        for (unsigned int j = 0; j < length; j++)
        {
            emit sendCurrentNoFProcess(300);
            TmpNumberOfDevces = (utilites::Convert8to16bits(bufferData + LastSizeValue) - 6) / 10;

//            qDebug() << "TmpNumberOfDevces: " << TmpNumberOfDevces;

            rawtime =
                (((unsigned char)bufferData[2 + LastSizeValue] << 24) |
                ((unsigned char)bufferData[3 + LastSizeValue] << 16) |
                ((unsigned char)bufferData[4 + LastSizeValue] << 8) |
                (unsigned char)bufferData[5 + LastSizeValue]);

            QDateTime timestamp;
            timestamp.setTimeSpec(Qt::UTC);
            timestamp.setTime_t(rawtime);
            tmp = timestamp.toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit();
            //Print time and date
            //;


            pThermoPck = new pckt_thermobraid[TmpNumberOfDevces];
            ArrayForSort = new int [TmpNumberOfDevces];

            fout << j + 1 << "\n";

            for (unsigned int k = 0; k < TmpNumberOfDevces; k++)
            {
                //Print number device

                //Print MAC address device
                for (int i = 0; i < 8; i++)
                {
                    pThermoPck[k].ow_mac[i] = (unsigned char)bufferData[(6 + i) + (k * 10) + LastSizeValue];
                    //fout << hex << uppercase << setfill('0') << setw(2) << pThermoPck[k].ow_mac[i];
                }

                //Print temperature device
                //fout << dec << '\t';
                pThermoPck[k].temperature =
                    (float)(((int16_t)((int16_t)bufferData[14 + (k * 10) + LastSizeValue] << 8) |
                    ((int16_t)bufferData[15 + (k * 10) + LastSizeValue])) * 0.0625);
                //fout << pThermoPck[k].temperature << "\r\n";
            }


            int CountBadMac = 0;

            for(unsigned int i = 0; i < TmpNumberOfDevces; i++)
            {
                ArrayForSort[i] = CalculateCoefAndFindPosition(pathpassport, &pThermoPck[i]);
                if ((ArrayForSort[i]) == 9999)
                {
                    ++CountBadMac;
                }
                else if ((ArrayForSort[i]) == -1)
                {
                    qDebug() << "File passport.txt not found in /mnt/hdd0/Data/Prototype/";
                    break;
                }
            }

            sorting(ArrayForSort, TmpNumberOfDevces);

            for (unsigned int i = 0; i < TmpNumberOfDevces - CountBadMac; i++)
            {
                for (unsigned int k = 0; k < TmpNumberOfDevces; k++)
                {
                    if (pThermoPck[k].NumberInPassport == ArrayForSort[i])
                    {
                        sprintf(bMacAddrField, "%02X%02X%02X%02X%02X%02X%02X%02X",
                            pThermoPck[k].ow_mac[0],pThermoPck[k].ow_mac[1],pThermoPck[k].ow_mac[2],pThermoPck[k].ow_mac[3],
                            pThermoPck[k].ow_mac[4],pThermoPck[k].ow_mac[5],pThermoPck[k].ow_mac[6],pThermoPck[k].ow_mac[7]);
                        fout << tmp.constData() << ";";                  //dtime
                        fout << dec << "Thermo" << ";";                //type_s
                        fout << dec << i + 1 << ";";               //num_p
                        fout << bMacAddrField << ";";                      //mac_addr
                        fout << dec << pThermoPck[k].temperature << ";";   //val_a
                        fout << (float)((float)(pThermoPck[k].temperature * pThermoPck[k].coef1) + pThermoPck[k].coef2);    //val_b
                        fout << "\n";

                        break;
                    }
                }
            }

            //For unknown or not found devices
            for (unsigned int w = 0; w < TmpNumberOfDevces; w++)
            {
                if (pThermoPck[w].NumberInPassport == 0)
                {
                    sprintf(bMacAddrField, "%02X%02X%02X%02X%02X%02X%02X%02X",
                            pThermoPck[w].ow_mac[0],pThermoPck[w].ow_mac[1],pThermoPck[w].ow_mac[2],pThermoPck[w].ow_mac[3],
                            pThermoPck[w].ow_mac[4],pThermoPck[w].ow_mac[5],pThermoPck[w].ow_mac[6],pThermoPck[w].ow_mac[7]);

                    fout << tmp.constData()               << ";";        //dtime
                    fout << dec << "Thermo"             << ";";        //type_s
                    fout << dec << 0                << ";";        //num_p
                    fout << bMacAddrField           << ";";        //mac_addr
                    fout << dec << pThermoPck[w].temperature << ";";
                    fout << dec << 0 << "\n";   //val_a
                }
            }

            LastSizeValue += (TmpNumberOfDevces * 10) + (6 + 4);

            delete[] ArrayForSort;
            delete[] pThermoPck;

//            qDebug() << "Thermodata write to txt end";

        }
        fout.close();

    }
    return 0;
}


int thermo::packageProcessing(QString pathdatalogs, uint8_t* bufferData, uint32_t length)
{
    ofstream fout;
    pckt_thermobraid *pThermoPck;
    time_t rawtime;
    struct tm timeinfo;
    unsigned int TmpNumberOfDevces = 0;
    unsigned int LastSizeValue = 0;
    char bDateTime[50], bMacAddrField[100];
    QString path2;
    QByteArray tmp;

    path2 = pathdatalogs + "/thermo";

    if (QDir(path2).exists() == false)
    {
        QDir().mkdir(path2);    //Создаем папку thermo
    }
    else
        qDebug() << "Already create folder";

    //Формируем txt файл с именем текущей даты и времени ПК
    path2 = path2 + '/' + QDateTime::currentDateTime().toString("ddMMyy_hhmmss") + ".txt";

    if (length <= 0)
    {
        return 1;
    }


    tmp = path2.toLocal8Bit();  //Преобразуем QString -> QByteArray -> const char*
    memset(bMacAddrField, 0, 100);


    fout.open(tmp.constData(), ios::app);
    if (!fout.is_open()) // если файл небыл открыт
    {
        qDebug() << "Файл thermo не может быть открыт или создан\n"; // напечатать соответствующее сообщение
        return -1; // выполнить выход из программы
    }
    else
    {
        qDebug() << "Файл thermo открыт или создан\n";


        for (unsigned int j = 0; j < length; j++)
        {
            TmpNumberOfDevces = (unsigned char)bufferData[4 + LastSizeValue];
            qDebug() << "TmpNumberOfDevces: " << TmpNumberOfDevces;

            rawtime =
                (((unsigned char)bufferData[0 + LastSizeValue] << 24) |
                ((unsigned char)bufferData[1 + LastSizeValue] << 16) |
                ((unsigned char)bufferData[2 + LastSizeValue] << 8) |
                (unsigned char)bufferData[3 + LastSizeValue]);
            memset(bDateTime, 0, 50);

            timeinfo = *gmtime(&rawtime);
            strftime(bDateTime, 50, "%F %X\0", &timeinfo);

            //Print time and date
            //;


            pThermoPck = new pckt_thermobraid[TmpNumberOfDevces];

            for (unsigned int k = 0; k < TmpNumberOfDevces; k++)
            {
                //Print number device
                //fout << dec << bDateTime << "\t" << k + 1 << '\t';

                //Print MAC address device
                for (int i = 0; i < 8; i++)
                {
                    pThermoPck[k].ow_mac[i] = (unsigned char)bufferData[(5 + i) + (k * 10) + LastSizeValue];
                    //fout << hex << uppercase << setfill('0') << setw(2) << pThermoPck[k].ow_mac[i];
                }

                //Print temperature device
                //fout << dec << '\t';
                pThermoPck[k].temperature =
                    (float)(((int16_t)((int16_t)bufferData[13 + (k * 10) + LastSizeValue] << 8) |
                    ((int16_t)bufferData[14 + (k * 10) + LastSizeValue])) * 0.0625);
                //fout << pThermoPck[k].temperature << "\r\n";
            }

            //For unknown or not found devices
            for (unsigned int w = 0; w < TmpNumberOfDevces; w++)
            {
                sprintf(bMacAddrField, "%02X%02X%02X%02X%02X%02X%02X%02X",
                            pThermoPck[w].ow_mac[0],pThermoPck[w].ow_mac[1],pThermoPck[w].ow_mac[2],pThermoPck[w].ow_mac[3],
                            pThermoPck[w].ow_mac[4],pThermoPck[w].ow_mac[5],pThermoPck[w].ow_mac[6],pThermoPck[w].ow_mac[7]);

                fout << bDateTime               << "\t";        //dtime
                fout << dec << "Thermo"             << "\t";        //type_s
                fout << dec << 0                << "\t";        //num_p
                fout << bMacAddrField           << "\t";        //mac_addr
                fout << dec << pThermoPck[w].temperature << "\t";   //val_a
                fout << dec << 0 << "\n";

            }

            LastSizeValue += (TmpNumberOfDevces * 10) + (4 + 1);

            delete[] pThermoPck;

            qDebug() << "Thermodata write to txt end";
        }
        fout.close();

    }
    return 0;
}

void thermo::sorting (int *Array, int LengthArray)
{
    int n = LengthArray;

    /* Отсортируем массив по возрастанию */
    for(int i = 1; i < n; ++i)
    {
        for(int r = 0; r < n-i; r++)
        {
            if(Array[r] > Array[r+1])
            {
                // Обмен местами
                int temp = Array[r];
                Array[r] = Array[r+1];
                Array[r+1] = temp;
            }
        }
    }
}


int thermo::CalculateCoefAndFindPosition(QString path, pckt_thermobraid *packet)
{
    fstream fout;
    char lineChar[100];
    char BufferMacAddresses[19] = {"\0"};
    char *start, *stop;
    uint32_t count = 0;
    QByteArray tmp = path.toLocal8Bit();
    fout.open(tmp.constData(), ios::in);

    if (!fout.is_open()) // если файл небыл открыт
    {
        qDebug() << "File not opened or can't be create\n"; // напечатать соответствующее сообщение
        return -1;
        //return 1; // выполнить выход из программы
    }
    sprintf(BufferMacAddresses, "%08X",
            packet->ow_mac[0] << 24 | packet->ow_mac[1] << 16 | packet->ow_mac[2] << 8 | packet->ow_mac[3]);
    sprintf(BufferMacAddresses+8, "%08X", packet->ow_mac[4] << 24 | packet->ow_mac[5] << 16 | packet->ow_mac[6] << 8 | packet->ow_mac[7]);

    //qDebug() << BufferMacAddresses << endl;

    if (!fout.is_open()) // если файл небыл открыт
    {
       // qDebug() << "File not opened or can't be create\n"; // напечатать соответствующее сообщение
        //return 1; // выполнить выход из программы
    }
    else
    {
        while (!(fout.eof()))
        {
            fout.getline(lineChar, 100, '\n');

            start = strstr(lineChar, "28");

            if (start != NULL)
            {
                ++count;

                start = strstr(lineChar, BufferMacAddresses);
                if (start != NULL)
                {
                    start = strchr(start, '\t');
                    if (start != NULL)
                    {
                        start[2] = '.';
                        packet->coef1 = strtod(start, &stop);
                        start = strchr(stop, ',');

                        if (start != NULL)
                        {
                            start[0] = '.';
                            //qDebug() << start1 << endl;
                        }

                        packet->coef2 = strtod(stop + 1, NULL);
                        //qDebug() << packet->coef1 << " " << packet->coef2 << endl;

                        packet->NumberInPassport = count;
                        fout.close(); // закрываем файл
                        return count;
                    }
                }
            }
        }
        fout.close(); // закрываем файл
    }
    packet->NumberInPassport = 0;
    return 9999;
}


