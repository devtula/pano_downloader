#include "google_pano_pix.h"

google_pano_pix::google_pano_pix(QObject *parent) :
    QObject(parent)
{
     init_flag=0;
}
google_pano_pix::~google_pano_pix(){

    if (init_flag==1){
        delete my_http_downloader;
        delete fullpix;
        delete p;

    }
}

void google_pano_pix::Init(){


    my_http_downloader = new http_downloader(this);


    fullpix= new QPixmap(13312,6656); //for QPainter magic
    p = new QPainter();

    connect(my_http_downloader, SIGNAL(done(const QByteArray&)),
            this,  SLOT(slotDone(const QByteArray&))
           );
    connect(my_http_downloader, SIGNAL(error()),
            this,  SLOT(slotError())
           );
    init_flag=1;

}




void google_pano_pix::slotError()
{


//    commit_downloaded_param(*my_http_downloader->_my_PanoParam); //need for stoping double download tile

//    qsrand(2);
//     my_http_downloader->_my_PanoParam->URL=QString(tr("http://geo%1.ggpht.com/cbk?output=tile&zoom=%2&x=%3&y=%4&cb_client=maps_sv&fover=0&onerr=0&renderer=spherical&v=4&panoid=%5").arg(qrand()%4).arg(my_http_downloader->_my_PanoParam->z).arg(my_http_downloader->_my_PanoParam->i).arg(my_http_downloader->_my_PanoParam->j).arg(my_http_downloader->_my_PanoParam->oui));

  //  my_http_downloader->_my_PanoParam->Copy(next_download_param());

    my_http_downloader->download();

}

void google_pano_pix::slotDone(const QByteArray& ba)
{
    mutex.lock();


    if (ba.size()==0){
        qDebug() << "not data";
        mutex.unlock();
        return;

    }
    //QPixmap * fullpixtemp = new QPixmap(13312,6656);




//    _param.URL=QString(tr("http://geo%1.ggpht.com/cbk?output=tile&zoom=%2&x=%3&y=%4&cb_client=maps_sv&fover=0&onerr=0&renderer=spherical&v=4&panoid=%5").arg(qrand()%4).arg(_param.z).arg(_param.i).arg(_param.j).arg(_param.oui));


    QPixmap * buttonImage = new QPixmap();
    buttonImage->loadFromData(ba);

 //   QPainter p(fullpixtemp); // создаём рисовалку и нацеливаем её на картинку
//    QPainter p(fullpix);
 //   p.drawPixmap(0,0,fullpix);

    if (fullpix->isNull()){
         qDebug()<<"fullpix is NULL";
         exit(0);

    }

if (!p->begin(fullpix)){
  qDebug()<<"p not active"<<LoadedTiles;
         //   exit(0);
}
    p->drawPixmap(my_http_downloader->_my_PanoParam->i*512,my_http_downloader->_my_PanoParam->j*512,*buttonImage); // накладываем изображение из файла на эту картинку в координаты 0,0

    if (!p->end()){
        qDebug()<<"p not ending"<<LoadedTiles;
    } // завершаем работу рисовалки

 //   fullpix=fullpixtemp->copy(0,0,fullpixtemp->width(),fullpixtemp->height());
 //   delete fullpixtemp;
//    delete p;
    delete buttonImage;
    LoadedTiles++;
    emit Current_Tiles_Load(QString("Loaded: %1 of %2 ;   i= %3 j= %4  oui = %5").arg(LoadedTiles).arg((my_http_downloader->_my_PanoParam->xmax+1)*(my_http_downloader->_my_PanoParam->ymax+1)).arg(my_http_downloader->_my_PanoParam->i).arg(my_http_downloader->_my_PanoParam->j).arg(my_http_downloader->_my_PanoParam->oui));
    commit_downloaded_param(*my_http_downloader->_my_PanoParam);

    if (next_download_param().oui=="NONE"){
        SaveFilePano(*my_http_downloader->_my_PanoParam);
        mutex.unlock();
        emit Pano_Next_Download(my_http_downloader->_my_PanoParam->oui);

    }else {

        mutex.unlock();
        my_http_downloader->_my_PanoParam->Copy(next_download_param());
        my_http_downloader->download();


    }

}

void google_pano_pix::get_google_pix(QString _ID){

    if (init_flag==0){

        Init();

    }

    LoadedTiles=0;
    int xmax=25;
    int ymax=12;

    fullpix->fill();

    clear_param();

    my_PanoParam  _param;


    for (int x=0;x<=xmax;x++)
        for (int y=0;y<=ymax;y++){
            //
            _param.i=x; _param.j=y; _param.z=5; _param.oui = _ID;
            _param.xmax=xmax;_param.ymax=ymax;
            _param.URL=QString(tr("http://geo%1.ggpht.com/cbk?output=tile&zoom=%2&x=%3&y=%4&cb_client=maps_sv&fover=0&onerr=0&renderer=spherical&v=4&panoid=%5").arg(qrand()%4).arg(_param.z).arg(_param.i).arg(_param.j).arg(_param.oui));
            insert_param(_param);
        }

    my_http_downloader->_my_PanoParam->Copy(next_download_param());

    my_http_downloader->download();

}




void  google_pano_pix::clear_param(){

    ParamMap.clear();


}

void google_pano_pix::insert_param(my_PanoParam  _param){


    ParamMap.insert(_param,0);
    return;

}

void google_pano_pix::commit_downloaded_param(const my_PanoParam&  _param){



    if (ParamMap.contains(_param)){
        ParamMap[_param]=1;
    }else{
        qDebug()<<"NO VALID PARAM!!!";}


}
my_PanoParam google_pano_pix::next_download_param(){


   return ParamMap.key(0);

}


inline bool operator<(const my_PanoParam& _pm1,const my_PanoParam& _pm2)
{
    //return (qHash(_pm1.URL)<qHash(_pm2.URL));
    return (_pm1.URL<_pm2.URL);
}

void google_pano_pix::SaveFilePano(const my_PanoParam &_param){
    QSettings settings("dev.tula.bz", "Pano downloader");
    QFile file(settings.value("path_for_downloaded_pano",QString("./pano")).toString()+_param.oui+".jpg");
    file.open(QIODevice::WriteOnly);
    //fullpix.save(&file, "JPG");
    fullpix->save(&file, "JPG");

//    delete p;
  //  delete fullpix;
  //  qDebug() <<"save"<<_param.oui;
    file.close();
}
