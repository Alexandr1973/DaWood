#include "mainwindow.h"

#include <QApplication>
#include <QtSerialPort/QtSerialPort>
#include <QDebug>
#include <QObject>


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    MainWindow w;

    w.show();
    return a.exec();
}
