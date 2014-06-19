#ifndef YANDEX_PANO_PIX_H
#define YANDEX_PANO_PIX_H

#include <QObject>
#include <QFile>
#include <QDebug>
#include <QPixmap>
#include <QPainter>
#include <QMutex>
#include <QMap>
#include <QCryptographicHash>
#include <QSettings>
#include "http_downloader.h"
#include "my_panoparam.h"

class yandex_pano_pix : public QObject
{
    Q_OBJECT
public:
    explicit yandex_pano_pix(QObject *parent = 0);
    ~yandex_pano_pix();
    void get_yandex_pix(QString,int,int,int);

signals:
    void Pano_Next_Download(QString);
    void Current_Tiles_Load(QString);


public slots:

    void slotError();
    void slotDone(const QByteArray&);

private:

    http_downloader* my_http_downloader;
    int LoadedTiles;

    int init_flag;
    int heightFullPix;


    QMap <my_PanoParam,int> ParamMap;

public:


   QPixmap fullpix;


private:

   void insert_param(my_PanoParam);
   void commit_downloaded_param(const my_PanoParam &);
   my_PanoParam next_download_param();
   void clear_param();
   void SaveFilePano(const my_PanoParam &);
   void Init();

};



#endif // YANDEX_PANO_PIX_H
