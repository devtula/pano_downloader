#include "yandex_pano_oid.h"

yandex_pano_oid::yandex_pano_oid(QObject *parent) :
    QObject(parent)
{
    my_http_downloader = new http_downloader(this);


    connect(my_http_downloader, SIGNAL(error()),
            this,  SLOT(slotError())
            );

    connect(my_http_downloader, SIGNAL(done(const QByteArray&)),
            this,  SLOT(slotDone(const QByteArray&))
            );

    couin_download_pano=0;
}

yandex_pano_oid::~yandex_pano_oid(){


    delete my_http_downloader;


}

void yandex_pano_oid::get_yandex_oid(QString _OID){

    // qDebug()<<"get_yandex_oid";

    my_PanoParam _param;
    _param.oui=_OID;
    //    _param.URL=QString("http://maps.yandex.ru/services/panoramas/1.x/panorama.xml?l=stv&lang=ru_RU&origin=maps&provider=streetview&oid=")+_OID;
    _param.URL=QString("http://panoramas.api-maps.yandex.ru/panorama/1.x/?l=stv&lang=ru_RU&oid=%1&origin=maps&provider=streetview").arg(_OID);
    my_http_downloader->_my_PanoParam->Copy(_param);
    my_http_downloader->download();


}


void yandex_pano_oid::check_downloaded_pano(QString path,int check_size_image){

    QString pano_id;
    QImage test_images;
    int tested_image_count = 0;

    my_my_sql = new my_sql();


    QDirIterator iterator (path, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while(iterator.hasNext())
    {
        qApp->processEvents();

        iterator.next();

        if (iterator.fileInfo().suffix()=="jpg"){
            tested_image_count++;
            pano_id = iterator.fileInfo().fileName().split(".")[0];
            if (pano_id.length()!=34){  //1298110759_689387421_23_1288610093
                qDebug() << "size filename of " <<iterator.fileInfo().absoluteFilePath() <<pano_id.length();
            }

            my_my_sql->readData(QString("SELECT * FROM yandex_pano_oids WHERE oid = '%1'; ").arg(pano_id));

            int count_oid_records = 0;

            if (check_size_image==1){

                test_images.load(iterator.fileInfo().absoluteFilePath());
            }



            while (my_my_sql->query->next()) {
                count_oid_records++;

                if (check_size_image==1){

                    if (test_images.height() != my_my_sql->query->value(my_my_sql->query->record().indexOf("Zoom_height")).toInt()){
                        qDebug()<<"!!! height "<< iterator.fileInfo().absoluteFilePath();
                    }
                    if (test_images.width() != my_my_sql->query->value(my_my_sql->query->record().indexOf("Zoom_width")).toInt()){
                        qDebug()<<"!!! width " << iterator.fileInfo().absoluteFilePath();
                    }
                }

            }


            my_my_sql->query->finish();

            if (count_oid_records==0) {
                qDebug() << " pano not found in db " <<iterator.fileInfo().absoluteFilePath();
            }

            if (count_oid_records==1) {

                QString query;
                query = "UPDATE yandex_pano_oids SET status = 2";
                query.append(QString(", local_path = '%1' ").arg(iterator.fileInfo().absoluteFilePath()));
                query.append(QString("WHERE oid = '%1' ;").arg(pano_id));

                my_my_sql->exec(query);
                my_my_sql->query->finish();

            }

            if (count_oid_records>1) {
                qDebug()<< "WHAT? count_oid_records is "<<count_oid_records <<" oids "<<pano_id ;
            }

            emit   Current_Oid_Load(QString(" testted %1 images").arg(tested_image_count));

            //   qDebug() << QString.split(".",iterator.fileInfo().fileName());
        }
    }
    my_my_sql->exec("UPDATE yandex_pano_oids SET status = 5 WHERE status = 6;"); // reset redownload status
    my_my_sql->query->finish();

    delete my_my_sql;
    return;
}

void yandex_pano_oid::download_ID_using_the_coordinates(QString lng,QString lat){

    QString url = QString("http://maps.yandex.ru/services/search/hotspots/1.x/hotspots.json?l=stv&ll=%1%3%2&lang=ru_RU&origin=maps").arg(lng,lat,"%2C");
    my_PanoParam _param;
    _param.URL = url;
    _param.flag = 1;

    my_http_downloader->_my_PanoParam->Copy(_param);
    my_http_downloader->download();
}

void yandex_pano_oid::slotError(){

    my_http_downloader->download();

}



void yandex_pano_oid::slotDone(const QByteArray& ba)
{


    couin_download_pano++;\

    if (my_http_downloader->_my_PanoParam->flag == 1) {

        QRegExp rx("\"id\":\"(.*)\"");
        rx.setMinimal(true);

        int pos = 0;
        while ((pos = rx.indexIn(ba, pos)) != -1)
        {
            get_yandex_oid(rx.cap(1));
            pos += rx.matchedLength();
        }
        return;

    }

    my_PanoParam _param;

    Insert_Data_To_Table(ba);

    //   QFile file("./oid/"+my_http_downloader->_my_PanoParam->oui+".xml");
    //   file.open(QIODevice::WriteOnly);
    //   file.write(ba);
    //   file.close();


    if (next_download_ya_oid().isEmpty()){
        emit Current_Oid_Load("NO MORE oid");

    }else{
        emit Current_Oid_Load(QString(" Downloaded  %1 - ").arg(couin_download_pano)+my_http_downloader->_my_PanoParam->oui);
        _param.oui=next_download_ya_oid();
        _param.URL=QString("http://panoramas.api-maps.yandex.ru/panorama/1.x/?l=stv&lang=ru_RU&oid=%1&origin=maps&provider=streetview").arg(_param.oui);
        //_param.URL=QString("http://maps.yandex.ru/services/panoramas/1.x/panorama.xml?l=stv&lang=ru_RU&origin=maps&provider=streetview&oid=")+_param.oui;
        my_http_downloader->_my_PanoParam->Copy(_param);
        my_http_downloader->download();

    }
}

//====================================================
void yandex_pano_oid::insert_ya_oid(QString oid_string){

    //if (!OIDHash.contains(oid_string)){
    //   OIDHash.insert(oid_string,0);
    //}

    my_my_sql = new my_sql(this);

    int count_oid_records = 0;

    my_my_sql->readData(QString("SELECT count(*) FROM yandex_pano_oids WHERE oid = '%1'; ").arg(oid_string));

    if (my_my_sql->query->next()){

        count_oid_records=my_my_sql->query->value(my_my_sql->query->record().indexOf("count(*)")).toInt();

    }
    my_my_sql->query->finish();


    if (count_oid_records==0) {

        my_my_sql->exec(QString("INSERT INTO yandex_pano_oids (oid , status) VALUES ('%1', '3');").arg(oid_string));
        my_my_sql->query->finish();
    }



    if (count_oid_records>1) {
        qDebug()<< "WHAT? count_oid_records is "<<count_oid_records <<" oids "<< oid_string;}

    delete        my_my_sql;

}

QString yandex_pano_oid::next_download_ya_oid(){
    //  return OIDHash.key(0);
    my_my_sql = new my_sql(this);

    QString returnString;


    my_my_sql->readData("SELECT * FROM yandex_pano_oids WHERE status = 3  OR status = 0 LIMIT 1;");

    if (my_my_sql->query->next()){
        returnString = my_my_sql->query->value(my_my_sql->query->record().indexOf("oid")).toString();
        my_my_sql->query->finish();
        return returnString;


    }else {
        my_my_sql->query->finish();
        return "";

    }

    delete my_my_sql;





}
//====================================================





void yandex_pano_oid::Insert_Data_To_Table(const QByteArray& ba)
{

    xml_oid_ya_Parser handler;

    QXmlInputSource *  source = new QXmlInputSource();

    source->setData(ba);
    QXmlSimpleReader  reader;


    reader.setContentHandler(&handler);
    reader.parse(source);

    delete source;


}



//###########################################################################

xml_oid_ya_Parser::xml_oid_ya_Parser()
{
    zoom_flag=0;
    node_flag=0;
    flag_data=0;
    my_my_sql = new my_sql();
}

xml_oid_ya_Parser::~xml_oid_ya_Parser()
{
    delete my_my_sql;
}


// ------------------------------------------------------------------
bool xml_oid_ya_Parser::startElement(const QString&,
                                     const QString&,
                                     const QString& str,
                                     const QXmlAttributes& attrs
                                     )
{

    if (str=="Data"){
        flag_data=1;
    }
    if (str=="Zoom"){
        zoom_flag=1;
    }
    if (str=="Node"){
        node_flag=1;
    }

    for(int i = 0; i < attrs.count(); i++) {

        if (attrs.localName(i)=="id"     && node_flag==1)   Node_id=attrs.value(i);
        if (attrs.localName(i)=="lon"    && node_flag==1)   Node_lon=attrs.value(i);
        if (attrs.localName(i)=="lat"    && node_flag==1)   Node_lat=attrs.value(i);
        if (attrs.localName(i)=="panoid" && node_flag==1)   Node_panoid=attrs.value(i);


        //            qDebug() << "localName "<< attrs.localName(i);
        //            qDebug() << "Attr:" << attrs.value(i);

    }
    return true;
}

// ------------------------------------------------------------------
bool xml_oid_ya_Parser::characters(const QString& strText)
{
    m_strText = strText;
    return true;
}

// ------------------------------------------------------------------
bool xml_oid_ya_Parser::endElement(const QString&, const QString&, const QString& str)
{

    if (str=="panorama"){
    }

    if (str=="Data"){


        QRegExp rx("oid=(.*)&%");
        rx.setMinimal(true);


        if( rx.indexIn(Data_href) != -1)
        {
            panoid=  rx.cap(1);

        }else {
            qDebug()<<"error get oid from href"<<Data_href;
        }
        QStringList words = Data_gml_pos.split(" ",QString::SkipEmptyParts);


        my_my_sql->readData(QString("SELECT * FROM yandex_pano_oids WHERE oid = '%1'; ").arg(panoid));

        int count_oid_records = 0;
        float lon1,lon2,lat1,lat2;

        while (my_my_sql->query->next()) {
            count_oid_records++;

            if (my_my_sql->query->value(my_my_sql->query->record().indexOf("lon")).toString()!=words[0]){

                lon1 = words[0].toFloat();
                lon2 = my_my_sql->query->value(my_my_sql->query->record().indexOf("lon")).toFloat();

                if ( qFabs(lon1-lon2) < 0.0001){

                    if (words[0].length()<my_my_sql->query->value(my_my_sql->query->record().indexOf("lon")).toString().length()){
                        words[0]=my_my_sql->query->value(my_my_sql->query->record().indexOf("lon")).toString();

                    }

                }else{
                    qDebug()<<panoid<<" Lon change!! new ="<<words[0]<<" old= "<<my_my_sql->query->value(my_my_sql->query->record().indexOf("lon")).toString();
                }
            }
            if (my_my_sql->query->value(my_my_sql->query->record().indexOf("lat")).toString()!=words[1]){
                lat1 = words[1].toFloat();
                lat2 = my_my_sql->query->value(my_my_sql->query->record().indexOf("lat")).toFloat();

                if ( qFabs(lat1-lat2) < 0.0001){
                    if (words[1].length() < my_my_sql->query->value(my_my_sql->query->record().indexOf("lat")).toString().length()){
                        words[1]=my_my_sql->query->value(my_my_sql->query->record().indexOf("lat")).toString();

                    }

                }else{
                    qDebug()<<panoid<<" Lon change!! new ="<<words[1]<<" old= "<<my_my_sql->query->value(my_my_sql->query->record().indexOf("lat")).toString();
                }
            }

        }

        my_my_sql->query->finish();

        if (count_oid_records==0) {
            my_my_sql->exec(QString("INSERT INTO yandex_pano_oids  (oid,status,timestamp,lon,lat,name,href,Zoom_height,Zoom_width)"
                                    "VALUES('%1', %2 ,%3, %4 , %5 , '%6', '%7',%8,%9);").arg(panoid).arg("1").arg(Data_timestamp).arg(words[0]).arg(words[1]).arg(Data_gml_name).arg(Data_href).arg(Zoom_height0).arg(Zoom_width0));

            my_my_sql->query->finish();


        }
        if (count_oid_records==1) {

            my_my_sql->exec(QString("UPDATE yandex_pano_oids SET status = %2, timestamp = %3, lon = %4 ,lat = %5 ,name = '%6',href = '%7',Zoom_height = %8,Zoom_width = %9  WHERE oid = '%1';").arg(panoid).arg("1").arg(Data_timestamp).arg(words[0]).arg(words[1]).arg(Data_gml_name).arg(Data_href).arg(Zoom_height0).arg(Zoom_width0));
            my_my_sql->query->finish();
        }

        if (count_oid_records>1) {
            qDebug()<< "WHAT? count_oid_records is "<<count_oid_records <<" oids "<<panoid ;}





        flag_data=0;
        panoid=""; Data_gml_pos="";Data_gml_name="";Data_timestamp="";Data_href="";

    }


    if (str=="Zoom" && Zoom_Level=="0"){
        zoom_flag=0;
        Zoom_width0=Zoom_width;
        Zoom_height0=Zoom_height;

        Zoom_Level="";
        Zoom_width="";
        Zoom_height="";
    }
    if (str=="Node"){
        node_flag=0;

        my_my_sql->readData(QString("SELECT * FROM yandex_pano_oids WHERE oid = '%1'; ").arg(Node_panoid));

        int count_oid_records = 0;
        float lon1,lon2,lat1,lat2;

        while (my_my_sql->query->next()) {
            count_oid_records++;


            if (my_my_sql->query->value(my_my_sql->query->record().indexOf("lon")).toString()!=Node_lon){

                lon1 = Node_lon.toFloat();
                lon2 = my_my_sql->query->value(my_my_sql->query->record().indexOf("lon")).toFloat();

                if ( qFabs(lon1-lon2) < 0.0001){


                    if (Node_lon.length()<my_my_sql->query->value(my_my_sql->query->record().indexOf("lon")).toString().length()){
                        Node_lon=my_my_sql->query->value(my_my_sql->query->record().indexOf("lon")).toString();

                    }

                }else{
                    qDebug()<<panoid<<" Lon change!! new ="<<Node_lon<<" old= "<<my_my_sql->query->value(my_my_sql->query->record().indexOf("lon")).toString();
                }
            }
            if (my_my_sql->query->value(my_my_sql->query->record().indexOf("lat")).toString()!=Node_lat){
                lat1 = Node_lat.toFloat();
                lat2 = my_my_sql->query->value(my_my_sql->query->record().indexOf("lat")).toFloat();
                if ( qFabs(lat1-lat2) < 0.0001){
                    if (Node_lat.length()<my_my_sql->query->value(my_my_sql->query->record().indexOf("lat")).toString().length()){
                        Node_lat=my_my_sql->query->value(my_my_sql->query->record().indexOf("lat")).toString();

                    }

                }else{
                    qDebug()<<panoid<<" Lon change!! new ="<<Node_lat<<" old= "<<my_my_sql->query->value(my_my_sql->query->record().indexOf("lat")).toString();
                }
            }


        }


        if (count_oid_records==0) {
            my_my_sql->exec(QString("INSERT INTO yandex_pano_oids  (oid,status, lon,lat) VALUES('%1',0, %2 ,%3);").arg(Node_panoid).arg(Node_lon).arg(Node_lat));
        } else if (count_oid_records!=1){
            qDebug()<< "WHAT? count_oid_records is "<<count_oid_records;}


        my_my_sql->query->finish();

        Node_id="";
        Node_lon="";
        Node_lat="";
        Node_panoid="";
    }



    if (zoom_flag==1&&str=="level"){

        Zoom_Level=m_strText;
    }
    if (zoom_flag==1&&str=="width"){

        Zoom_width=m_strText;
    }
    if (zoom_flag==1&&str=="height"){

        Zoom_height=m_strText;
    }

    if (flag_data==1){

        if (str=="gml:name"){
            Data_gml_name=m_strText;
        }

        if (str=="href"){
            Data_href=m_strText;
        }


        if (str=="gml:pos"){
            Data_gml_pos=m_strText;
        }


        if (str=="timestamp"){
            Data_timestamp=m_strText;
        }
    }

    return true;
}

// ------------------------------------------------------------------
bool xml_oid_ya_Parser::fatalError(const QXmlParseException& exception)
{
    qDebug() << "Line:"      << exception.lineNumber()
             << ", Column:"  << exception.columnNumber()
             << ", Message:" << exception.message();
    return false;
}
