#include "yandex_pano_pix.h"

yandex_pano_pix::yandex_pano_pix(QObject *parent) :
    QObject(parent)
{
    init_flag=0;

}
yandex_pano_pix::~yandex_pano_pix(){

    if (init_flag==1)     delete my_http_downloader;
}

void yandex_pano_pix::Init(){


    my_http_downloader = new http_downloader(this);

    connect(my_http_downloader, SIGNAL(done(const QByteArray&)),
            this,  SLOT(slotDone(const QByteArray&))
           );
    connect(my_http_downloader, SIGNAL(error()),
            this,  SLOT(slotError())
           );

    init_flag=1;

}

void yandex_pano_pix::slotError()
{
    my_http_downloader->_my_PanoParam->Copy(next_download_param());
    my_http_downloader->download();
}



void yandex_pano_pix::slotDone(const QByteArray& ba)
{

   //  qDebug()<<"slot done"<<LoadedTiles<<my_http_downloader->_my_PanoParam->URL;

 /*/    if (ba[1]!='Ø'){ //FF D8 FF
            qDebug() << "not jpg";
            qDebug()<<ba;
            mutex.unlock();
            return;
 }/*/

       if (ba.size()==0){
             qDebug() << "not data";

             return;

       }

       QPixmap * fullpixtemp = new QPixmap(5632,heightFullPix);
//       QPixmap fullpixtemp(5632,heightFullPix);

//       //fullpixtemp.scaled(fullpix.width(),fullpix.height());

       QPixmap buttonImage;
       buttonImage.loadFromData(ba);

       QPainter p(fullpixtemp); // создаём рисовалку и нацеливаем её на картинку
       p.drawPixmap(0,0,fullpix);
       p.drawPixmap(my_http_downloader->_my_PanoParam->i*256,my_http_downloader->_my_PanoParam->j*256,buttonImage); // накладываем изображение из файла на эту картинку в координаты 0,0
       p.end(); // завершаем работу рисовалки


       fullpix=fullpixtemp->copy(0,0,fullpixtemp->width(),fullpixtemp->height());
       delete fullpixtemp;

//       fullpixtemp.~QPixmap();


       LoadedTiles++;
       emit Current_Tiles_Load(QString("Loaded: %1 of %2 ;   i= %3 j= %4  oui = %5").arg(LoadedTiles).arg((my_http_downloader->_my_PanoParam->xmax+1)*(my_http_downloader->_my_PanoParam->ymax+1)).arg(my_http_downloader->_my_PanoParam->i).arg(my_http_downloader->_my_PanoParam->j).arg(my_http_downloader->_my_PanoParam->oui));
       commit_downloaded_param(*my_http_downloader->_my_PanoParam);

        if (next_download_param().oui=="NONE"){

            SaveFilePano(*my_http_downloader->_my_PanoParam);
            emit Pano_Next_Download(my_http_downloader->_my_PanoParam->oui);

        }else {
            my_http_downloader->_my_PanoParam->Copy(next_download_param());
            my_http_downloader->download();

        }

}




void yandex_pano_pix::get_yandex_pix(QString _OID,int xmax,int ymax,int heightPix){

    if (init_flag==0){

        Init();

    }
    LoadedTiles=0;
    fullpix.fill();
    if (!fullpix.isNull()){
        if (fullpix.height()!=heightPix){

            fullpix.scaledToHeight(heightPix);
            qDebug()<<"other height"<<fullpix.height() ;
        }}

    fullpix.fill();

    heightFullPix=heightPix;
    clear_param();

    my_PanoParam _param;


    for (int x=0;x<=xmax;x++)
        for (int y=0;y<=ymax;y++){
            //
            _param.i=x; _param.j=y; _param.z=0; _param.oui = _OID;
            _param.xmax=xmax;_param.ymax=ymax;
            _param.URL=QString(tr("http://stv0%5.maps.yandex.net/images/?v=2.9.0&oid=%1&x=%2&y=%3&z=%4").arg(_OID).arg(x).arg(y).arg(_param.z).arg(qrand()%4+1));

            //_param.URL=QString(tr("http://localhost/256_random_noice.png#&oid=%1&x=%2&y=%3&z=%4?%5").arg(_param.oui).arg(_param.i).arg(_param.j).arg(_param.z).arg(qrand()%4+1));

            insert_param(_param);


        }

    my_http_downloader->_my_PanoParam->Copy(next_download_param());
    my_http_downloader->download();

}

void  yandex_pano_pix::clear_param(){
    ParamMap.clear();

}

void yandex_pano_pix::insert_param(my_PanoParam  _param){


    ParamMap.insert(_param,0);
    return;

}

void yandex_pano_pix::commit_downloaded_param(const my_PanoParam&  _param){



    if (ParamMap.contains(_param)){
        ParamMap[_param]=1;
    }else{
        qDebug()<<"NO VALID PARAM!!!";}


}
my_PanoParam yandex_pano_pix::next_download_param(){


   return ParamMap.key(0);

}


inline bool operator<(const my_PanoParam& _pm1,const my_PanoParam& _pm2)
{
    //return (qHash(_pm1.URL)<qHash(_pm2.URL));
    return (_pm1.URL<_pm2.URL);
}

void yandex_pano_pix::SaveFilePano(const my_PanoParam &_param){
    QSettings settings("dev.tula.bz", "Pano downloader");
    QFile file(settings.value("path_for_downloaded_Ya_pano",QString("./pano")).toString()+_param.oui+".jpg");
    file.open(QIODevice::WriteOnly);
    fullpix.save(&file, "JPG");
    qDebug() <<"save"<<_param.oui;
    file.close();
}
