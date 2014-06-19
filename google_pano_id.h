#ifndef GOOGLE_PANO_ID_H
#define GOOGLE_PANO_ID_H

#include <QObject>
#include <QFile>
#include <QDebug>
#include <QtXml>
#include <QMutex>
#include <QDirIterator>

#include "my_sql.h"
#include "http_downloader.h"
#include "my_panoparam.h"
#include "google_pano_pix.h"



class xml_oid_go_Parser : public QXmlDefaultHandler {


public:

    explicit xml_oid_go_Parser();
    ~xml_oid_go_Parser();

private:
    QString m_strText;

    int zoom_flag;
    int node_flag;


    QString pano_id;
    QString image_date;
    QString lng;
    QString lat;
    QString original_lng;
    QString original_lat;
    QString elevation_wgs84_m;
    QString image_width;
    QString image_height;
    QString tile_width;
    QString tile_height;
    QString imagery_type;
    QString level_id;
    QString num_zoom_levels;
    QString road_argb;
    QString scene;
    QString copyright;
    QString pano_text;
    QString attribution_name;
    QString street_range;
    QString region;
    QString country;



//link
    QString pano_id_link;
    QString yaw_deg_link;
    QString road_argb_link;
    QString scene_link;
    QString link_text;

//projection_properties
    QString projection_type;
    QString pano_yaw_deg;
    QString tilt_yaw_deg;
    QString tilt_pitch_deg;
    QString vertical_scale;


//photo
    QString docid;
    QString yaw;
    QString pitch;
    QString zoom;
    QString author;
    QString description;
    QString authorid;
    QString is_matched;

//photo url
    QString name_url;
    QString value_url;

//geopoint for distance
    double lat_real;
    double lng_real;



private:
    QString RemoveBadChars(QString);


public:

    bool startElement(const QString&, const QString&, const QString&, const QXmlAttributes& );
    bool characters(const QString& );
    bool endElement(const QString&, const QString&, const QString& );
    bool fatalError(const QXmlParseException& exception);


    my_sql* my_my_sql;
};


class google_pano_id : public QObject
{
    Q_OBJECT
public:
    explicit google_pano_id(QObject *parent = 0);
        void get_data_from_folder(QString path);
        QString get_not_loaded_oid();
        void download_oid_where_status_zero();
        void download_id_from_koord(QString,QString, QString);
        void check_downloaded_pano(QString);


          my_sql* my_my_sql;
private:
          QMutex mutex;
          http_downloader* my_http_downloader;


          QFile data;
          QTextStream out;

signals:

public slots:
          void slotError();
          void slotDone(const QByteArray&);

};






#endif // GOOGLE_PANO_ID_H
