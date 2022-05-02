#ifndef PC_DATABASE_H
#define PC_DATABASE_H

#include <QObject>
#include <QtSql>
#include <QtSql/QSqlDatabase>

class pc_database
{
public:
    pc_database();
    ~pc_database();
    void writeParameter(int, QString);
    QString getParameter(int);
    bool checkParameter(int i);
    void updateParameter(int, QString);
    bool LoadDataInFile(QString namefile, QString table, QString column);

private:
    QSqlDatabase *db;
    QSqlQuery* query;
};

#endif // PC_DATABASE_H
