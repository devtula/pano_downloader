#ifndef GOOGLE_PANO_PIX_H
#define GOOGLE_PANO_PIX_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QPixmap>
#include <QPainter>
#include <QMutex>
#include <QDateTime>
#include <QSettings>


#include "http_downloader.h"
#include "my_panoparam.h"

class google_pano_pix : public QObject
{
    Q_OBJECT
public:
    explicit google_pano_pix(QObject *parent = 0);
    ~google_pano_pix();

    void get_google_pix(QString);


signals:
    void Pano_Next_Download(QString);
    void Current_Tiles_Load(QString);

public slots:


    void slotError();
    void slotDone(const QByteArray&);

private:
        int init_flag;
        int LoadedTiles;
        http_downloader* my_http_downloader;
        QMap <my_PanoParam,int> ParamMap;
       // QPixmap fullpix;
        QPixmap* fullpix;
        QPainter* p;
        QMutex mutex;

private:

   void insert_param(my_PanoParam);
   void commit_downloaded_param(const my_PanoParam &);
   my_PanoParam next_download_param();
   void clear_param();
   void SaveFilePano(const my_PanoParam &);
   void Init();


};

#endif // GOOGLE_PANO_PIX_H
