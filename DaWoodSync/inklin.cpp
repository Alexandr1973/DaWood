#include "inklin.h"

using namespace std;

inklin::inklin()
{

}


int inklin::PackageProcessing_Inklin(QString pathdatalogs, uint8_t* bufferData, uint32_t length)
{
    ofstream fout;
    time_t rawtime;
    unsigned int TmpNumberOfDevces = 0;
    unsigned int LastSizeValue = 0;
    inklinData *pInklinPck;
    char bMacAddrField[100];

    QString path2;
    QByteArray tmp;

    if (length <= 0)
    {
        return 1;
    }

    path2 = pathdatalogs + "/inklin";

    if (QDir(path2).exists() == false)
    {
        QDir().mkdir(path2);    //Создаем папку thermo
    }
    else
        qDebug() << "Folder Inklin already create";

    //Формируем txt файл с именем текущей даты и времени ПК
    path2 = path2 + '/' + QDateTime::currentDateTime().toString("ddMMyy_hhmmss") + ".csv";




    tmp = path2.toLocal8Bit();  //Преобразуем QString -> QByteArray -> const char*


    fout.open(tmp.constData(), ios::app);
    if (!fout.is_open()) // если файл небыл открыт
    {
        qDebug() << "Файл inklin не может быть открыт или создан\n"; // напечатать соответствующее сообщение
        //return 1; // выполнить выход из программы
    }
    else
    {
        qDebug() << "Файл inklin открыт или создан\n";


        //pthread_mutex_lock(&lock);

        memset(bMacAddrField, 0, 100);
        for (unsigned int j = 0; j < length; j++)
        {
            TmpNumberOfDevces = (unsigned char)bufferData[4 + LastSizeValue];
            qDebug() << "TmpNumberOfDevces: " << TmpNumberOfDevces;

            rawtime =
                (((unsigned char)bufferData[0 + LastSizeValue] << 24) |
                ((unsigned char)bufferData[1 + LastSizeValue] << 16) |
                ((unsigned char)bufferData[2 + LastSizeValue] << 8) |
                (unsigned char)bufferData[3 + LastSizeValue]);
            QDateTime timestamp;
            timestamp.setTimeSpec(Qt::UTC);
            timestamp.setTime_t(rawtime);
            tmp = timestamp.toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit();

            pInklinPck = new inklinData[TmpNumberOfDevces];

            for (unsigned int k = 0; k < TmpNumberOfDevces; k++)
            {
                //Print number device
                //fout << dec << bDateTime << "\t" << k + 1 << '\t';

                //Print MAC address device
                for (int i = 0; i < 8; i++)
                {
                    pInklinPck[k].serialnum[i] = (unsigned char)bufferData[(5 + i) + (k * 14) + LastSizeValue];
                    //fout << hex << uppercase << setfill('0') << setw(2) << pInklinPck[k].serialnum[i];
                }

                //Print temperature device
                //fout << dec << '\t';
                pInklinPck[k].axisX =
                    ((int16_t)((int16_t)bufferData[13 + (k * 14) + LastSizeValue] << 8) |
                    ((int16_t)bufferData[14 + (k * 14) + LastSizeValue]));

                pInklinPck[k].axisY =
                    ((int16_t)((int16_t)bufferData[15 + (k * 14) + LastSizeValue] << 8) |
                    ((int16_t)bufferData[16 + (k * 14) + LastSizeValue]));

                pInklinPck[k].axisZ =
                    ((int16_t)((int16_t)bufferData[17 + (k * 14) + LastSizeValue] << 8) |
                    ((int16_t)bufferData[18 + (k * 14) + LastSizeValue]));
                //fout << pInklinPck[k].temperature << "\r\n";
            }

            for (unsigned int k = 0; k < TmpNumberOfDevces; k++)
            {
                sprintf(bMacAddrField, "%02X%02X%02X%02X%02X%02X%02X%02X",
                    pInklinPck[k].serialnum[0],pInklinPck[k].serialnum[1],pInklinPck[k].serialnum[2],pInklinPck[k].serialnum[3],
                    pInklinPck[k].serialnum[4],pInklinPck[k].serialnum[5],pInklinPck[k].serialnum[6],pInklinPck[k].serialnum[7]);

                fout << tmp.constData() << "\t";                  	//dtime
//                fout << dec << hole_id << "\t";             			//hole_id
                fout << dec << "Inklin" << "\t";                //type_s
//                fout << dec << nbIndicationIntoDB << "\t";    	//num_i
                fout << dec << k + 1 << "\t";    				//num_p
                fout << bMacAddrField << "\t";					//mac_addr
                fout << dec << pInklinPck[k].axisX << "\t";   	//val_a
                fout << dec << pInklinPck[k].axisY << "\t";   	//val_b
                fout << dec << pInklinPck[k].axisZ << "\t";   	//val_c
                fout << "\n";
            }

            LastSizeValue += (TmpNumberOfDevces * 14) + (4 + 1);

            delete[] pInklinPck;

            qDebug() << "Thermodata write to txt end";
        }
        fout.close();
    }
    return 0;
}
