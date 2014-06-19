#ifndef YANDEX_PANO_OID_H
#define YANDEX_PANO_OID_H

#include <QObject>
#include <QFile>
#include <QDebug>
#include <QtXml>
#include <QImage>
#include "http_downloader.h"
#include "my_panoparam.h"
#include "my_sql.h"

class xml_oid_ya_Parser : public QXmlDefaultHandler {


public:

    explicit xml_oid_ya_Parser();
    ~xml_oid_ya_Parser();


  //  my_Pano_Yandex_OID_Param _my_Pano_Yandex_OID_Param_For_Nodes;


private:
    QString m_strText;

    int zoom_flag;
    int node_flag;
    int flag_data;

    QString Zoom_Level;
    QString Zoom_width;
    QString Zoom_height;
    QString Zoom_width0;
    QString Zoom_height0;

    QString Node_id;
    QString Node_lon;
    QString Node_lat;
    QString Node_panoid;

    QString panoid;
    QString Data_gml_pos;
    QString Data_gml_name;
    QString Data_timestamp;
    QString Data_href;


  //  my_Pano_Yandex_OID_Param _my_Pano_Yandex_OID_Param;



public:

    bool startElement(const QString&, const QString&, const QString&, const QXmlAttributes& );
    bool characters(const QString& );
    bool endElement(const QString&, const QString&, const QString& );
    bool fatalError(const QXmlParseException& exception);


    my_sql* my_my_sql;


};

class yandex_pano_oid : public QObject
{
    Q_OBJECT
public:
    explicit yandex_pano_oid(QObject *parent = 0);
    ~yandex_pano_oid();





private slots:
   // void slotGo              (                              );
   // void slotError           (                              );
   // void slotDownloadProgress(qint64, qint64                );

      void slotError();
      void slotDone            (const QByteArray&);

signals:
     void Current_Oid_Load(QString);


private:
     http_downloader* my_http_downloader;
  //   QHash <QString, int> OIDHash;


     void insert_ya_oid(QString oid_string);
 //    void commit_dowloaded_ya_oid(QString _OID);
     QString next_download_ya_oid();



     void Insert_Data_To_Table            (const QByteArray&             );


public:

   my_sql* my_my_sql;

   int couin_download_pano;

   void get_yandex_oid(QString);
   void download_ID_using_the_coordinates(QString,QString);
   void check_downloaded_pano(QString,int check_size_image=0);
};


#endif // YANDEX_PANO_OID_H
