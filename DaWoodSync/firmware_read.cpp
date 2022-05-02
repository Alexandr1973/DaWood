#include "firmware_read.h"

firmware_read::firmware_read()
{

}

using namespace std;

int firmware_read::getLength (QString pathfirmware)
{
    QString path2 = pathfirmware;
    long length = 0;
    QByteArray tmp;

    tmp = path2.toLocal8Bit();  //Преобразуем QString -> QByteArray -> const char*

    fstream fs (tmp.constData(), ios::in | ios::binary);

    if (!fs)
    {
        qDebug() << "Firmware not find";
        length = -1;
    }
    else
    {
        fs.clear();

        fs.seekg(0, ios::end);
        length = fs.tellg();
        fs.seekg(0, ios::beg);

        qDebug() << "Length payloads: " << length;
        fs.close();
    }
    return length;
}

int firmware_read::getFirmware (QString pathfirmware, uint8_t *firmware)
{
    QString path2 = pathfirmware;
    QByteArray tmp;
    int res = 0;
    unsigned int i = 0;
    char c;
    tmp = path2.toLocal8Bit();  //Преобразуем QString -> QByteArray -> const char*

    fstream fs (tmp.constData(), ios::in | ios::binary);
    if (!fs)
    {
        qDebug() << "Firmware not find";
        res = -1;
    }
    else
    {
        while (!fs.eof())
        {
            fs.get(c);
            firmware[i] = c;
            i++;
        }

        fs.close();
        res = 1;
    }
    return res;
}
