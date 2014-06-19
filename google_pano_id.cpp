#include "google_pano_id.h"

google_pano_id::google_pano_id(QObject *parent) :
    QObject(parent)
{



    my_http_downloader = new http_downloader(this);

    connect(my_http_downloader, SIGNAL(done(const QByteArray&)),
            this,  SLOT(slotDone(const QByteArray&))
           );


    data.setFileName("output_google_ids.txt");
    if (!data.open(QIODevice::WriteOnly | QIODevice::Append)){
        qDebug()<<"file not open for write"; return;}


    out.setDevice(&data);

}
void google_pano_id::slotError()
{
  //  my_http_downloader->_my_PanoParam->Copy(next_download_param());
  //  my_http_downloader->download();
}


void google_pano_id::slotDone(const QByteArray& ba)
{
    mutex.lock();


    xml_oid_go_Parser handler;
    QXmlInputSource   source;
    source.setData(ba);
    QXmlSimpleReader  reader;

    reader.setContentHandler(&handler);
    reader.parse(source);

    out<<"\n=============DATA NAME: "<<my_http_downloader->_my_PanoParam->oui<<"\n";

    out<<QString(ba);
    out.flush();
    mutex.unlock();


    download_oid_where_status_zero();
}

void google_pano_id::get_data_from_folder(QString path){


       int i;
        QDir my_dir;



        QString filename;

        my_dir.setCurrent(path);
        //my_dir.setCurrent("/home/user/workspace2/qt-projects/yandex_get_oid/build/yandex_get_oid-Desktop_Qt_5_2_1_GCC_64bit-Debug/id/");
        QStringList stlist=my_dir.entryList();

        while(!stlist.isEmpty()) {

            filename = stlist.takeFirst();
            if ((filename=="..") or (filename==".")){continue;}
            i++;

            xml_oid_go_Parser handler;
            QFile             file(filename);
            QXmlInputSource   source(&file);
            QXmlSimpleReader  reader;

            reader.setContentHandler(&handler);
            reader.parse(source);

            out<<"\n=============DATA NAME: "<<filename<<"\n";
            file.seek(0);
            out<<QString(file.readAll());
            out.flush();
            file.remove();
        }



        exit(0);
    return;
}


QString google_pano_id::get_not_loaded_oid(){

    my_my_sql = new my_sql();  // need move to konstructor
    my_my_sql->readData(QString("SELECT * FROM google_pano_ids WHERE status = 0 LIMIT 1;"));
    if (my_my_sql->query->next()){
    return my_my_sql->query->value(my_my_sql->query->record().indexOf("pano_id")).toString();
    }else{
        return "None!";
    }
}




void google_pano_id::download_oid_where_status_zero(){
    my_my_sql = new my_sql();
    my_PanoParam _param;
    QString _ID;

    my_my_sql->readData(QString("SELECT * FROM google_pano_ids WHERE status = 0 LIMIT 1;"));


    if (my_my_sql->query->next()){

        _ID = my_my_sql->query->value(my_my_sql->query->record().indexOf("pano_id")).toString();

        _param.oui=_ID;
        _param.URL=QString("http://geo%1.ggpht.com/cbk?output=xml&cb_client=maps_sv&hl=ru&dm=1&pm=1&ph=1&renderer=cubic,spherical&v=4&panoid=").arg(qrand()%4)+_ID;
        my_http_downloader->_my_PanoParam->Copy(_param);
        my_http_downloader->download();
        return; // if not work LIMIT 1 (0_0)
    }else {

        qDebug()<<"ids over";
    }
    delete my_my_sql;
}

void google_pano_id::download_id_from_koord(QString lat,QString lng, QString radius){


    my_PanoParam _param;
    _param.URL= QString("http://maps.google.com/cbk?output=xml&hl=ru&ll=%1,%2&radius=%3&cb_client=maps_sv&v=4&it=all").arg(lat).arg(lng).arg(radius);

    //qDebug()<<_param.URL;
    my_http_downloader->_my_PanoParam->Copy(_param);
    my_http_downloader->download();

}


void google_pano_id::check_downloaded_pano(QString path){

    QString pano_id;

    my_my_sql = new my_sql();



    QDirIterator iterator (path, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while(iterator.hasNext())

    {
        qApp->processEvents();

        iterator.next();

        if (iterator.fileInfo().suffix()=="jpg"){
                  pano_id = iterator.fileInfo().fileName().split(".")[0];
                  if (pano_id.length()!=22){
                      qDebug() << "size filename of " <<iterator.fileInfo().absoluteFilePath() <<pano_id.length();
                  }

                  my_my_sql->readData(QString("SELECT * FROM google_pano_ids WHERE pano_id = '%1'; ").arg(pano_id));

                  int count_oid_records = 0;


                  while (my_my_sql->query->next()) {
                      count_oid_records++;

                  }


                  my_my_sql->query->finish();

                  if (count_oid_records==0) {

                      qDebug() << " pano not found in db " <<iterator.fileInfo().absoluteFilePath() <<pano_id.length();


                  }
                  if (count_oid_records==1) {


                      QString query;
                      query = "UPDATE google_pano_ids SET status = 2";
                      query.append(QString(", local_path = '%1' ").arg(iterator.fileInfo().absoluteFilePath()));
                      query.append(QString("WHERE pano_id = '%1' ;").arg(pano_id));

                      my_my_sql->exec(query);
                      my_my_sql->query->finish();


                  }

                  if (count_oid_records>1) {
                      qDebug()<< "WHAT? count_oid_records is "<<count_oid_records <<" oids "<<pano_id ;}

               //   qDebug() << QString.split(".",iterator.fileInfo().fileName());
        }
    }
    my_my_sql->exec("UPDATE google_pano_ids SET status = 5 WHERE status = 6;"); // reset redownload status
    my_my_sql->query->finish();

    delete my_my_sql;
    return;
}




// ------------------------------------------------------------------


xml_oid_go_Parser::xml_oid_go_Parser(){

my_my_sql = new my_sql();

}


xml_oid_go_Parser::~xml_oid_go_Parser(){

delete my_my_sql;

}



bool xml_oid_go_Parser::startElement(const QString&,
                                     const QString&,
                                     const QString& str,
                                     const QXmlAttributes& attrs
                                     )
{



    for(int i = 0; i < attrs.count(); i++) {


        if (str == "data_properties"){

            if ( attrs.localName(i)=="image_width"){
                image_width=RemoveBadChars(attrs.value(i));}

            if ( attrs.localName(i)=="image_height"){
                image_height=RemoveBadChars(attrs.value(i));}

            if ( attrs.localName(i)=="tile_width"){
                tile_width=RemoveBadChars(attrs.value(i));}

            if ( attrs.localName(i)=="tile_height"){
                tile_height=RemoveBadChars(attrs.value(i));}

            if ( attrs.localName(i)=="image_date"){
                image_date=RemoveBadChars(attrs.value(i));}

            if ( attrs.localName(i)=="pano_id"){
                if (!pano_id.isEmpty()){qDebug()<<"ALARM!! pano_id not emply!"<<pano_id;}
                pano_id=RemoveBadChars(attrs.value(i));}

            if ( attrs.localName(i)=="scene"){
                scene=RemoveBadChars(attrs.value(i));}

            if ( attrs.localName(i)=="imagery_type"){
                imagery_type=RemoveBadChars(attrs.value(i));}

            if ( attrs.localName(i)=="level_id"){
                level_id=RemoveBadChars(attrs.value(i));}

            if ( attrs.localName(i)=="num_zoom_levels"){
                num_zoom_levels=RemoveBadChars(attrs.value(i));}

            if ( attrs.localName(i)=="lat"){
                lat=RemoveBadChars(attrs.value(i));
                lat_real = lat.toDouble();
            }

            if ( attrs.localName(i)=="lng"){
                lng=RemoveBadChars(attrs.value(i));
                lng_real = lng.toDouble();
            }

            if ( attrs.localName(i)=="original_lat"){
                original_lat=RemoveBadChars(attrs.value(i));}

            if ( attrs.localName(i)=="original_lng"){
                original_lng=RemoveBadChars(attrs.value(i));}

            if ( attrs.localName(i)=="elevation_wgs84_m"){
                elevation_wgs84_m =RemoveBadChars(attrs.value(i));}


        }
        if (str=="projection_properties"){


            if ( attrs.localName(i)=="projection_type"){
                projection_type =RemoveBadChars(attrs.value(i));}

            if ( attrs.localName(i)=="pano_yaw_deg"){
                pano_yaw_deg =RemoveBadChars(attrs.value(i));}

            if ( attrs.localName(i)=="tilt_yaw_deg"){
                tilt_yaw_deg =RemoveBadChars(attrs.value(i));}

            if ( attrs.localName(i)=="tilt_pitch_deg"){
                tilt_pitch_deg =RemoveBadChars(attrs.value(i));}

            if ( attrs.localName(i)=="vertical_scale"){
                vertical_scale =RemoveBadChars(attrs.value(i));}

        }
        if (str=="link"){

            if ( attrs.localName(i)=="pano_id"){
                pano_id_link =RemoveBadChars(attrs.value(i));}

            if ( attrs.localName(i)=="yaw_deg"){
                yaw_deg_link =RemoveBadChars(attrs.value(i));}

            if ( attrs.localName(i)=="road_argb"){
                road_argb_link =RemoveBadChars(attrs.value(i));}

            if ( attrs.localName(i)=="scene"){
                scene_link =RemoveBadChars(attrs.value(i));}
        }
        if (str=="photo"){

            if ( attrs.localName(i)=="docid"){
                docid =RemoveBadChars(attrs.value(i));}

            if ( attrs.localName(i)=="yaw"){
                yaw = RemoveBadChars(attrs.value(i));}


            if ( attrs.localName(i)=="pitch"){
                pitch = RemoveBadChars(attrs.value(i));}

            if ( attrs.localName(i)=="zoom"){
                zoom = RemoveBadChars(attrs.value(i));}

            if ( attrs.localName(i)=="author"){
                author = RemoveBadChars(attrs.value(i));}

            if ( attrs.localName(i)=="description"){
                description = RemoveBadChars(attrs.value(i));}

            if ( attrs.localName(i)=="authorid"){
                authorid = RemoveBadChars(attrs.value(i));}

            if ( attrs.localName(i)=="is_matched"){
                is_matched = RemoveBadChars(attrs.value(i));}


        }

        if (str=="url"){
            if ( attrs.localName(i)=="name"){
                name_url =RemoveBadChars(attrs.value(i));}

            if ( attrs.localName(i)=="value"){
                value_url =RemoveBadChars(attrs.value(i));}


        }

                 //   qDebug() << "Attr:" << RemoveBadChars(attrs.value(i));

    }


    return true;
}

// ------------------------------------------------------------------
bool xml_oid_go_Parser::characters(const QString& strText)
{
    m_strText = RemoveBadChars(strText);
    return true;
}

// ------------------------------------------------------------------
bool xml_oid_go_Parser::endElement(const QString&, const QString&, const QString& str)
{
    if (str=="copyright"){
        copyright=m_strText;}

    if (str=="text"){
        pano_text=m_strText;}

    if (str=="street_range"){
        street_range=m_strText;}

    if (str=="region"){
        region=m_strText;}

    if (str=="country"){
        country=m_strText;}

    if (str=="attribution_name"){
        attribution_name=m_strText;}

    if (str=="data_properties"){



        my_my_sql->readData(QString("SELECT * FROM google_pano_ids WHERE pano_id = '%1'; ").arg(pano_id));

        int count_oid_records = 0;


        while (my_my_sql->query->next()) {
            count_oid_records++;

        }


        my_my_sql->query->finish();

        if (count_oid_records==0) {

            QString query;
            query = "INSERT INTO google_pano_ids"
                    "(pano_id , status , image_date";

            if (!lng.isEmpty()){        query.append(", lng ");}
            if (!lat.isEmpty()){        query.append(", lat ");}
            if (!original_lng.isEmpty()){        query.append(", original_lng ");}
            if (!original_lat.isEmpty()){        query.append(",original_lat  ");}
            if (!elevation_wgs84_m.isEmpty()){        query.append(", elevation_wgs84_m ");}

            query.append(",image_width , image_height , tile_width , tile_height , imagery_type , "
                    "level_id , num_zoom_levels , road_argb , scene , "
                    "copyright , pano_text , attribution_name, street_range , region , country) ");
            query.append(QString("VALUES('%1' ,").arg(pano_id));
            query.append(QString("1 ,")); //status
            query.append(QString("'%1' ,").arg(image_date));


            if (!lng.isEmpty()){ query.append(QString(" %1 ,").arg(lng));}else{ qDebug()<<"Alarm!! not lng"<<pano_id;;}
            if (!lat.isEmpty()){ query.append(QString(" %1 ,").arg(lat));}else{qDebug()<<"Alarm!! not lat"<<pano_id;}
            if (!original_lng.isEmpty()){ query.append(QString(" %1 ,").arg(original_lng));}else{qDebug()<<"Alarm!! not original_lng"<<pano_id;;}
            if (!original_lat.isEmpty()){ query.append(QString(" %1 ,").arg(original_lat));}else{qDebug()<<"Alarm!! not original_lat"<<pano_id;;}
            if (!elevation_wgs84_m.isEmpty()){ query.append(QString(" %1 ,").arg(elevation_wgs84_m));}

            query.append(QString(" %1 , %2 , %3 , %4 , %5 ,").arg(image_width).arg(image_height).arg(tile_width).arg(tile_height).arg(imagery_type));
            query.append(QString(" '%1' , %2 , '%3' , '%4' , ").arg(level_id).arg(num_zoom_levels).arg(road_argb).arg(scene));
            query.append(QString(" '%1' , '%2' , '%3' , '%4' ,'%5', '%6' );").arg(copyright).arg(pano_text).arg(attribution_name).arg(street_range).arg(region).arg(country));


            my_my_sql->exec(query);
            my_my_sql->query->finish();


        }
        if (count_oid_records==1) {
            // need add check what data update/

            QString query;
            query = "UPDATE google_pano_ids SET status = 1";
            query.append(QString(", image_date = '%1' ").arg(image_date));

            if (!lng.isEmpty()){ query.append(QString(",lng = %1 ").arg(lng));}else{ qDebug()<<"Alarm!! not lng"<<pano_id;;}
            if (!lat.isEmpty()){ query.append(QString(",lat = %1 ").arg(lat));}else{ qDebug()<<"Alarm!! not lat"<<pano_id;}
            if (!original_lng.isEmpty()){ query.append(QString(",original_lng = %1 ").arg(original_lng));}else{ qDebug()<<"Alarm!! not original_lng"<<pano_id;;}
            if (!original_lat.isEmpty()){ query.append(QString(",original_lat = %1 ").arg(original_lat));}else{ qDebug()<<"Alarm!! not original_lat"<<pano_id;;}
            if (!elevation_wgs84_m.isEmpty()){ query.append(QString(",elevation_wgs84_m = %1 ").arg(elevation_wgs84_m));}

            query.append(QString(", image_width = %1 ,image_height = %2 ,tile_width = %3 ,tile_height = %4 ,imagery_type = %5 ").arg(image_width).arg(image_height).arg(tile_width).arg(tile_height).arg(imagery_type));
            query.append(QString(", level_id = '%1' ,num_zoom_levels = %2 ,road_argb = '%3' ,scene = '%4'  ").arg(level_id).arg(num_zoom_levels).arg(road_argb).arg(scene));
            query.append(QString(", copyright = '%1' , pano_text = '%2' , attribution_name = '%3' , street_range = '%4' ,region = '%5',country = '%6' ").arg(copyright).arg(pano_text).arg(attribution_name).arg(street_range).arg(region).arg(country));

            query.append(QString("WHERE pano_id = '%1' ;").arg(pano_id));

            my_my_sql->exec(query);
            my_my_sql->query->finish();


        }

        if (count_oid_records>1) {
            qDebug()<< "WHAT? count_oid_records is "<<count_oid_records <<" oids "<<pano_id ;}


        my_my_sql->query->finish();

     //    pano_id="";
         image_date="";
         lng="";
         lat="";
         original_lng="";
         original_lat="";
         elevation_wgs84_m="";
         image_width="";
         image_height="";
         tile_width="";
         tile_height="";
         imagery_type="";
         level_id="";
         num_zoom_levels="";
         road_argb="";
         scene="";
         copyright="";
         pano_text="";
         street_range="";
         region="";
         country="";
       }


    if (str=="projection_properties"){

        my_my_sql->exec(QString("INSERT INTO google_pano_id_projection_properties (pano_id , projection_type , pano_yaw_deg , tilt_yaw_deg , tilt_pitch_deg , vertical_scale) VALUES('%1', '%2', '%3', '%4', '%5', '%6');").arg(pano_id).arg(projection_type).arg(pano_yaw_deg).arg(tilt_yaw_deg).arg(tilt_pitch_deg).arg(vertical_scale));
        my_my_sql->query->finish();

        projection_type="";
        pano_yaw_deg="";
        tilt_yaw_deg="";
        tilt_pitch_deg="";
        vertical_scale="";

    }



    if (str=="link_text"){
        link_text=m_strText;}


    if (str=="link"){



        my_my_sql->readData(QString("SELECT * FROM google_pano_ids WHERE pano_id = '%1'; ").arg(pano_id_link));

        int count_oid_records = 0;



        while (my_my_sql->query->next()) {
            count_oid_records++;

        }

        if (count_oid_records==0) {

            //cheking distance about point

            float lat=54.19;
            float lon=37.62;

            double DEG_TO_RAD = 0.017453292519943295769236907684886;
            double EARTH_RADIUS_IN_METERS = 6372797.560856;
            double distance = EARTH_RADIUS_IN_METERS* 2.0 * asin(sqrt(sin((lat -lat_real) * DEG_TO_RAD * 0.5)*sin((lat -lat_real) * DEG_TO_RAD * 0.5) + \
                                                                      cos(lat*DEG_TO_RAD) * cos(lat_real*DEG_TO_RAD)*sin((lon - lng_real) * DEG_TO_RAD * 0.5)*sin((lon - lng_real) * DEG_TO_RAD * 0.5)));

            if (distance<20000){
                my_my_sql->exec(QString("INSERT INTO google_pano_ids  (pano_id, status ) VALUES('%1',0);").arg(pano_id_link));
            }else{
                my_my_sql->exec(QString("INSERT INTO google_pano_ids  (pano_id, status ) VALUES('%1',4);").arg(pano_id_link));
            }

        } else if (count_oid_records!=1){
            qDebug()<< "WHAT? count_oid_records is "<<count_oid_records;}



        my_my_sql->exec(QString("INSERT INTO google_pano_id_link (pano_id, pano_id_parent, yaw_deg, road_argb, scene, link_text ) VALUES('%1', '%2', '%3', '%4', '%5', '%6');").arg(pano_id_link).arg(pano_id).arg(yaw_deg_link).arg(road_argb_link).arg(scene_link).arg(link_text));
        my_my_sql->query->finish();


        pano_id_link="";
        yaw_deg_link="";
        road_argb_link="";
        scene_link="";
        link_text="";

    }
    if (str=="photo"){


        my_my_sql->readData(QString("SELECT * FROM google_pano_ids WHERE pano_id = '%1'; ").arg(pano_id_link));

        int count_oid_records = 0;

        while (my_my_sql->query->next()) {
            count_oid_records++;
        }

        if (count_oid_records==0) {

            my_my_sql->exec(QString("INSERT INTO google_pano_id_photo "
                                    "(docid, pano_id, yaw, pitch, zoom, author, description, authorid, is_matched )"
                                    " VALUES('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8' , '%9');"
                                    "").arg(docid).arg(pano_id).arg(yaw).arg(pitch).arg(zoom).arg(author).arg(description).arg(authorid).arg(is_matched));

        } else if (count_oid_records!=1){
            qDebug()<< "WHAT? count_oid_records is "<<count_oid_records<< " docid =  "<<docid ;}





        my_my_sql->query->finish();

        docid="";
        yaw="";
        pitch="";
        zoom="";
        author="";
        description="";
        authorid="";
        is_matched="";
    }
    if (str=="url"){

        my_my_sql->readData(QString("SELECT * FROM google_pano_id_photo_url WHERE docid = '%1' AND name = '%2'; ").arg(docid).arg(name_url));

        int count_oid_records = 0;


        while (my_my_sql->query->next()) {
            count_oid_records++;
        }

        if (count_oid_records==0) {

            my_my_sql->exec(QString("INSERT INTO google_pano_id_photo_url "
                                    "(docid, pano_id, name, value )"
                                    " VALUES('%1', '%2', '%3', '%4');"
                                    "").arg(docid).arg(pano_id).arg(name_url).arg(value_url));

        } else if (count_oid_records!=1){
            qDebug()<< "WHAT? count_oid_records is "<<count_oid_records<< " docid =  "<<docid ;}



        my_my_sql->query->finish();
        name_url="";
        value_url="";

    }


    if (str=="panorama"){

        qApp->processEvents();
        pano_id="";


    }



  m_strText="";
  //  qDebug()<< "End element"<<str;
  //  qDebug()<< m_strText;
    return true;
}


bool xml_oid_go_Parser::fatalError(const QXmlParseException& exception)
{
qDebug() << "Line:"      << exception.lineNumber()
     << ", Column:"  << exception.columnNumber()
     << ", Message:" << exception.message();
return false;
}

    QString xml_oid_go_Parser::RemoveBadChars(QString str){

        str.replace("\'","`");
        str.replace("\\","|");
        str.replace("/","|");
        str.replace("\"","``");
    return    str;
    }
