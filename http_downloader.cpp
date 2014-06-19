#include "http_downloader.h"

http_downloader::http_downloader(QObject *parent) :
    QObject(parent)
{
        m_pnam = new QNetworkAccessManager(this);
        _my_PanoParam = new my_PanoParam(this);

        connect(m_pnam, SIGNAL(finished(QNetworkReply*)),
                this,   SLOT(slotFinished(QNetworkReply*))
               );
        timer = new QTimer(this);
        timer->setInterval(120000);
        timer->setSingleShot(false);
        connect(timer, SIGNAL(timeout()), SLOT(slotTimeoutHTTP()));
        flag_timeOut=0;




}
http_downloader::~http_downloader(){

    delete _my_PanoParam;
    delete timer;
    delete m_pnam;


}



void http_downloader::download()
{
    QNetworkRequest * request = new QNetworkRequest(_my_PanoParam->URL);

    pnr = m_pnam->get(*request);
    timer->stop();
    timer->start();

    delete request;
}

void http_downloader::slotFinished(QNetworkReply* pnr)
{
    timer->stop();
    flag_timeOut=0;
    if (pnr->error() != QNetworkReply::NoError) {


      //  qDebug()<<QDateTime::currentDateTime().toString(Qt::ISODate)<<"ERROR!!! "<<pnr->errorString();
        if (pnr->size()>0){
        //    qDebug()<< pnr->readAll();
        }

        m_pnam->clearAccessCache();
        pnr->abort();
        pnr->deleteLater();

        emit error();
    }
    else {
        if (pnr->size()>2){
        emit done(pnr->readAll());}
        else {
            qDebug()<<QDateTime::currentDateTime().toString(Qt::ISODate)<<"ERROR!!! NO Data.";
             m_pnam->clearAccessCache();
            pnr->abort();
            pnr->deleteLater();

            emit error();
        }
    }


}

void http_downloader::slotTimeoutHTTP() {
    timer->stop();
   // qDebug()<<QDateTime::currentDateTime().toTimeSpec(Qt::OffsetFromUTC).toString(Qt::ISODate)<<"ERROR!!! Time Out";
 //   qDebug()<<QDateTime::currentDateTime().toString(Qt::ISODate)<<"ERROR!!! Time Out";

    if (flag_timeOut==0){
        flag_timeOut=1;
        m_pnam->clearAccessCache();
        pnr->abort();
        m_pnam->deleteLater();
    }
    emit error();


}
