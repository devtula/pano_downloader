#ifndef MY_SQL_H
#define MY_SQL_H

#include <QObject>
#include <QSql>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QDebug>

class my_sql : public QObject
{
    Q_OBJECT
public:
    explicit my_sql(QObject *parent = 0);

    QSqlError addConnection(const QString &, const QString &, const QString &,
                  const QString &, const QString &, int = -1);


    void readData(QString);


    QSqlQuery * query;
    QSqlDatabase db;



signals:
    void statusMessage(const QString &message);

public slots:
    void exec(QString );

};

#endif // MY_SQL_H
