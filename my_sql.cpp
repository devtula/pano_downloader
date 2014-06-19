#include "my_sql.h"

my_sql::my_sql(QObject *parent) :
    QObject(parent)
{

query = new QSqlQuery(db);

}
void my_sql::exec(QString textQuery)
{

    query->exec(textQuery);

    if (query->lastError().type() != QSqlError::NoError){
        qDebug()<<query->lastError().text();
        qDebug()<<"text Query"<<textQuery;
        emit statusMessage(query->lastError().text());

    }else if (query->isSelect())
        emit statusMessage(tr("Query OK."));
    else
        emit statusMessage(tr("Query OK, number of affected rows: %1").arg(query->numRowsAffected()));



  //  this->query=query;

}


void my_sql::readData(QString textQuery){

  // qDebug()<<textQuery;


    if (! query->exec(textQuery)) {
        qDebug()<<query->lastError().text();
        qDebug()<<"text Query"<<textQuery;
        emit statusMessage(query->lastError().text());

        return;
    }

    if (query->lastError().type() != QSqlError::NoError){
        qDebug()<<query->lastError().text();
        qDebug()<<"text Query"<<textQuery;
        emit statusMessage(query->lastError().text());

        query->record();

        return;
    }
    else if (query->isSelect()){
        emit statusMessage(tr("Query OK."));
        query->record();
        return;


    }
    else{
        emit statusMessage(tr("Query OK, number of affected rows: %1").arg(query->numRowsAffected()));
        query->record();
        return;

    }

}



QSqlError my_sql::addConnection(const QString &driver, const QString &dbName, const QString &host,
                            const QString &user, const QString &passwd, int port)
{
    QSqlError err;
    db = QSqlDatabase::addDatabase(driver);
    db.setDatabaseName(dbName);
    db.setHostName(host);
    db.setPort(port);
    if (!db.open(user, passwd)) {
        err = db.lastError();
        emit statusMessage(tr("Error Open"));
    } else {
        statusMessage(tr("db Opened!"));
    }

    query = new QSqlQuery(db);


    return err;
}
