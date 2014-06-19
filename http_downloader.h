#ifndef HTTP_DOWNLOADER_H
#define HTTP_DOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QFile>
#include "my_panoparam.h"


class http_downloader : public QObject
{
    Q_OBJECT
public:
    explicit http_downloader(QObject *parent = 0);
    ~http_downloader();


private:
    QNetworkAccessManager* m_pnam;
    QNetworkReply*  pnr;

    QTimer* timer;

    int flag_timeOut;

public:


    void download();

signals:
    void downloadProgress(qint64, qint64                );
    void done            (const QByteArray&             );
    void error           (                              );
    void downloaded      (http_downloader *             );


private slots:
    void slotFinished(QNetworkReply*);
    void slotTimeoutHTTP();

public:
    my_PanoParam * _my_PanoParam;
};

#endif // HTTP_DOWNLOADER_H
