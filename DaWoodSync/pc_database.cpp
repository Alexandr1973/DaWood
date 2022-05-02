#include "pc_database.h"

pc_database::pc_database()
{
    QTemporaryDir dir;
    QString path = dir.path().left(dir.path().indexOf("/DaWood"));
    path = path + "/DaWoodSeriesSync";

    if (QDir(path).exists() == false)
    {
        QDir().mkdir(path);
    }
    else
        qDebug() << "Already create folder";

    path = path + "/dawoodpc.sqlite";

    db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
    db->setDatabaseName(path);
    db->open();
    query = new QSqlQuery(*db);
    query->exec("CREATE TABLE setting (id INTEGER, parameter varchar(255));");
    query->exec("CREATE TABLE `indications` (`id` INTEGER PRIMARY KEY AUTOINCREMENT,"
                             "`dtime` text DEFAULT 0,"
                             "`hole_id` INTEGER DEFAULT 0,"
                             "`type_s` varchar(255) DEFAULT NULL,"
                             "`num_i` INTEGER unsigned NOT NULL,"
                             "`num_p` INTEGER unsigned DEFAULT '0',"
                             "`mac_addr` varchar(255) DEFAULT 0,"
                             "`val_a` float DEFAULT '0',"
                             "`val_b` float DEFAULT '0',"
                             "`val_c` float DEFAULT '0',"
                             "`val_d` float DEFAULT '0',"
                             "`val_e` float DEFAULT '0',"
                             "`val_f` float DEFAULT '0',"
                             "`date_time` text DEFAULT 0);");
}

pc_database::~pc_database()
{
    db->close();
}

bool pc_database::checkParameter(int i)
{
    query->prepare( "SELECT * FROM setting WHERE id = ?" );
    query->addBindValue(i); // assuming idx is an integer/long/QVariant value
    if (!query->exec())
    {
        qDebug() << "Error" << query->lastError();
    }
    return query->next();
}

void pc_database::writeParameter(int i, QString parameter)
{
    if (checkParameter(i) == false)
    {
        query->prepare("INSERT INTO setting (id, parameter) "
                          "VALUES (:id, :parameter)");
        query->bindValue(":id", i);
        query->bindValue(":parameter", parameter);
        query->exec();
    }
    else
    {
        updateParameter(i, parameter);
    }

}

QString pc_database::getParameter(int i)
{
    query->prepare( "SELECT * FROM setting WHERE id = ?" );
    query->addBindValue(i); // assuming idx is an integer/long/QVariant value
    if (!query->exec())
    {
        qDebug() << "Error" << query->lastError();
    }
    query->first();
    return query->value("parameter").toString();
}

void pc_database::updateParameter(int i, QString parameter)
{
    query->prepare("UPDATE setting SET parameter=:parameter "
                      "WHERE id=:id");
    query->bindValue(":id", i);
    query->bindValue(":parameter", parameter);
    query->exec();
}



bool pc_database::LoadDataInFile(QString namefile, QString table, QString column)
{
    query->prepare("LOAD DATA INFILE \":namefile\" INTO TABLE " + table + '(' + column + ");");
    query->bindValue(":namefile", namefile);
    return query->exec();
}
