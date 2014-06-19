#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

      readSettings();

    my_yandex_pano_pix = new yandex_pano_pix(this);
    my_yandex_pano_oid = new yandex_pano_oid(this);
    my_google_pano_id = new google_pano_id(this);
    my_google_pano_pix = new google_pano_pix(this);


    my_my_sql = new my_sql(this);



    SQL_init();

    connect(my_my_sql, SIGNAL(statusMessage(QString)), ui->statusBar, SLOT(showMessage(QString)));

  //  ui->statusBar(), SLOT(showMessage(QString)));

    connect(my_yandex_pano_pix, SIGNAL(Current_Tiles_Load(QString)),ui->statusBar, SLOT(showMessage(QString)));

  //    this,  SLOT(Pano_status(QString)));
    connect(my_yandex_pano_pix, SIGNAL(Pano_Next_Download(QString)),this,  SLOT(Pano_Next_Download_Y(QString)));

    connect(my_google_pano_pix, SIGNAL(Current_Tiles_Load(QString)),ui->statusBar, SLOT(showMessage(QString)));
    connect(my_google_pano_pix, SIGNAL(Pano_Next_Download(QString)),this,  SLOT(Pano_Next_Download_G(QString)));

  //  connect(my_google_pano_pix, SIGNAL(Pano_Next_Download(QString)),this,  SLOT(Pano_Next_ReDownload_google(QString)));

   // connect(my_yandex_pano_oid, SIGNAL (Current_Oid_Load(QString)),ui->statusBar, SLOT(showMessage(QString)));
    connect(my_yandex_pano_oid, SIGNAL (Current_Oid_Load(QString)),this, SLOT(Pano_status(QString)));


    status_for_download_gpano=5;

}

MainWindow::~MainWindow()
{
    writeSettings();



    //delete _ConfigDialog;

    delete ui;

}


void MainWindow::readSettings(){
    QSettings settings("dev.tula.bz", "Pano downloader");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    search_path_for_downloaded_pano  = settings.value("search_path_for_downloaded_pano",QString("./pano")).toString();
    search_path_for_downloaded_Ya_pano= settings.value("search_path_for_downloaded_Ya_pano",QString("./pano")).toString();

    path_for_downloaded_pano= settings.value("path_for_downloaded_pano",QString("./pano")).toString();
    path_for_downloaded_Ya_pano= settings.value("path_for_downloaded_Ya_pano",QString("./pano")).toString();

    pano_id_database = settings.value("pano_id_database",QString("pano_ids.db3")).toString();


    ui->lineEdit->setText(settings.value("start lng ",QString("37.615")).toString()); //lng
    ui->lineEdit_2->setText(settings.value("start lat ",QString("54.19")).toString()); //lat
    ui->lineEdit_3->setText(settings.value("radius ",QString("20000")).toString());
    ui->lineEdit_4->setText(settings.value("delta lng ",QString("0.015")).toString());
    ui->lineEdit_5->setText(settings.value("delta lat ",QString("0.01")).toString());

    resize(size);
    move(pos);

}
void MainWindow::writeSettings(){

    QSettings settings("dev.tula.bz", "Pano downloader");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("search_path_for_downloaded_pano",search_path_for_downloaded_pano );
    settings.setValue("search_path_for_downloaded_Ya_pano",search_path_for_downloaded_Ya_pano );

    settings.setValue("path_for_downloaded_pano",path_for_downloaded_pano );
    settings.setValue("path_for_downloaded_Ya_pano",path_for_downloaded_Ya_pano );


    settings.setValue("pano_id_database",pano_id_database );

    //add checking for correct value

    settings.setValue("start lng ",ui->lineEdit->text());
    settings.setValue("start lat ",ui->lineEdit_2->text());
    settings.setValue("radius ",ui->lineEdit_3->text());
    settings.setValue("delta lng ",ui->lineEdit_4->text());
    settings.setValue("delta lat ",ui->lineEdit_5->text());

}

void MainWindow::SQL_init()// SQL init
{


    QString qs = QSqlDatabase::database().connectionName();

    my_my_sql->db.close();
    my_my_sql->db= QSqlDatabase();  //qsql magic
    my_my_sql->db.removeDatabase(qs);


    my_my_sql->addConnection("QSQLITE",pano_id_database,"","","");


    my_my_sql->exec("CREATE TABLE IF NOT EXISTS google_pano_ids ( "
                    "pano_id TEXT, "
                    "status INTEGER, "
                    "image_date TEXT, "
                    "lng REAL, "
                    "lat REAL, "
                    "original_lng REAL, "
                    "original_lat REAL, "
                    "elevation_wgs84_m REAL, "
                    "image_width INTEGER, "
                    "image_height INTEGER, "
                    "tile_width INTEGER, "
                    "tile_height INTEGER, "
                    "imagery_type INTEGER, "
                    "level_id TEXT, "
                    "num_zoom_levels INTEGER, "
                    "road_argb TEXT, "
                    "scene TEXT, "
                    "copyright TEXT, "
                    "pano_text TEXT, "
                    "attribution_name TEXT, "
                    "street_range TEXT, "
                    "region TEXT, "
                    "country TEXT, "
                    "local_path TEXT"
                    ");");

    my_my_sql->query->finish();
    my_my_sql->exec("CREATE UNIQUE INDEX IF NOT EXISTS 'pano_id_unique_index' ON 'google_pano_ids' ('pano_id');");
    my_my_sql->exec("CREATE INDEX IF NOT EXISTS 'status_index' ON 'google_pano_ids' ('status');");



    my_my_sql->exec("CREATE TABLE IF NOT EXISTS google_pano_id_link ( "
                    "pano_id TEXT, "
                    "pano_id_parent TEXT, "
                    "yaw_deg TEXT, "
                    "road_argb TEXT, "
                    "scene TEXT ,"
                    "link_text TEXT"
                    ");");

    my_my_sql->exec("CREATE TABLE IF NOT EXISTS google_pano_id_projection_properties ( "
                    "pano_id TEXT, "
                    "projection_type TEXT, "
                    "pano_yaw_deg REAL, "
                    "tilt_yaw_deg REAL, "
                    "tilt_pitch_deg REAL, "
                    "vertical_scale REAL"
                    ");");

    my_my_sql->exec("CREATE TABLE IF NOT EXISTS google_pano_id_photo ( "
                    "docid TEXT, "
                    "pano_id TEXT, "
                    "yaw REAL, "
                    "pitch REAL, "
                    "zoom REAL, "
                    "author TEXT, "
                    "description TEXT, "
                    "authorid INTEGER, "
                    "is_matched TEXT"
                    ");");

    my_my_sql->exec("CREATE INDEX IF NOT EXISTS 'docid_photo_unique_index' ON 'google_pano_id_photo' ('docid');");



    my_my_sql->exec("CREATE TABLE IF NOT EXISTS google_pano_id_photo_url (docid TEXT, pano_id TEXT, name TEXT, value TEXT);");

    my_my_sql->exec("CREATE INDEX IF NOT EXISTS 'docid_url_unique_index' ON 'google_pano_id_photo_url' ('docid');");
    my_my_sql->exec("CREATE INDEX IF NOT EXISTS 'name_url_unique_index' ON 'google_pano_id_photo_url' ('name');");


    my_my_sql->exec("CREATE TABLE IF NOT EXISTS  yandex_pano_oids( "
                             "oid TEXT, "
                             "status INTEGER, "
                             "timestamp INTEGER, "
                             "lon REAL, "
                             "lat REAL, "
                             "name TEXT, "
                             "href TEXT, "
                             "Zoom_height INTEGER, "
                             "Zoom_width INTEGER, "
                             "local_path TEXT"
                             ");");

    my_my_sql->exec("CREATE INDEX IF NOT EXISTS 'oid_unique_index' ON 'yandex_pano_oids' ('oid');");

}


void MainWindow::Disable_all_gui_element(){

    ui->lineEdit->setDisabled(true);
    ui->lineEdit_2->setDisabled(true);
    ui->lineEdit_3->setDisabled(true);
    ui->lineEdit_4->setDisabled(true);
    ui->lineEdit_5->setDisabled(true);

    ui->pushButton->setDisabled(true);
    ui->pushButton_2->setDisabled(true);
    ui->pushButton_3->setDisabled(true);
    ui->pushButton_4->setDisabled(true);
    ui->pushButton_5->setDisabled(true);
    ui->pushButton_6->setDisabled(true);
    ui->pushButton_7->setDisabled(true);
    ui->pushButton_8->setDisabled(true);
    ui->pushButton_9->setDisabled(true);
    ui->pushButton_10->setDisabled(true);
    ui->checkBox->setDisabled(true);
}

void MainWindow::Enable_all_gui_element(){

    ui->lineEdit->setEnabled(true);
    ui->lineEdit_2->setEnabled(true);
    ui->lineEdit_3->setEnabled(true);
    ui->lineEdit_4->setEnabled(true);
    ui->lineEdit_5->setEnabled(true);

    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    ui->pushButton_4->setEnabled(true);
    ui->pushButton_5->setEnabled(true);
    ui->pushButton_6->setEnabled(true);
    ui->pushButton_7->setEnabled(true);
    ui->pushButton_8->setEnabled(true);
    ui->pushButton_9->setEnabled(true);
    ui->pushButton_10->setEnabled(true);
    ui->checkBox->setEnabled(true);
}


void MainWindow::Pano_status(QString text){

    ui->statusBar->showMessage(text);

}

void MainWindow::Pano_Next_Download_Y(QString pano_id)
{

    QString query;

    query=QString("UPDATE yandex_pano_oids SET status = %1 WHERE oid = '%2' ;").arg(status_for_download_gpano).arg(pano_id);
    my_my_sql->exec(query);
    my_my_sql->query->finish();

    if (status_for_download_gpano==5)  on_pushButton_3_clicked();
    if (status_for_download_gpano==6)  on_pushButton_5_clicked();


}

void MainWindow::Pano_Next_Download_G(QString pano_id)
{


    QString query;



        query=QString("UPDATE google_pano_ids SET status = %1 WHERE pano_id = '%2' ;").arg(status_for_download_gpano).arg(pano_id);
        my_my_sql->exec(query);
        my_my_sql->query->finish();

        if (status_for_download_gpano==5)  on_pushButton_6_clicked();
        if (status_for_download_gpano==6)  on_pushButton_9_clicked();


}


void MainWindow::on_pushButton_clicked() //Get start Ya pano id
{

    Disable_all_gui_element();
    my_yandex_pano_oid->download_ID_using_the_coordinates(ui->lineEdit->text(),ui->lineEdit_2->text());

}




void MainWindow::on_pushButton_2_clicked()//Check downloaded Ya pano
{

    Disable_all_gui_element();
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 search_path_for_downloaded_Ya_pano ,
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty()){
        search_path_for_downloaded_Ya_pano=dir;
    }

    if (ui->checkBox->isChecked()){
        my_yandex_pano_oid->check_downloaded_pano(dir,1);
    }else {
    my_yandex_pano_oid->check_downloaded_pano(dir);
    }
  //  my_google_pano_id->check_downloaded_pano(dir);


    Enable_all_gui_element();

}


void MainWindow::on_pushButton_4_clicked() //Check downloaded pano
{
    Disable_all_gui_element();
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 search_path_for_downloaded_pano ,
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty()){
        search_path_for_downloaded_pano=dir;
    }

    my_google_pano_id->check_downloaded_pano(dir);

  //  my_google_pano_id->get_data_from_folder("./id/");
    Enable_all_gui_element();
}

void MainWindow::on_pushButton_3_clicked()// download ya pano
{
    Disable_all_gui_element();
    status_for_download_gpano=5;
    flag_GY=2;
    QString _ID;

    int ya_height;

    my_my_sql->readData(QString("SELECT * FROM yandex_pano_oids WHERE  status = 1 LIMIT 1;"));



    if (my_my_sql->query->next()){

        _ID = my_my_sql->query->value(my_my_sql->query->record().indexOf("oid")).toString();
        ya_height = my_my_sql->query->value(my_my_sql->query->record().indexOf("Zoom_height")).toInt();
        my_yandex_pano_pix->get_yandex_pix(_ID,21,7,ya_height);
    //    my_google_pano_pix->get_google_pix(_ID);

        return; // if not work LIMIT 1 (0_0)
    }else {

        Pano_status("ids over");
        Enable_all_gui_element();
    }

}


void MainWindow::on_pushButton_6_clicked()   // download g pano
{
    //--2hHyep6Yjzzx-3DsI-5w

    Disable_all_gui_element();

    status_for_download_gpano=5;
    flag_GY=1;


    QString _ID;
   // my_my_sql = new my_sql();
    float lat1 = ui->lineEdit_2->text().toFloat() - ui->lineEdit_5->text().toFloat();
    float lat2 = ui->lineEdit_2->text().toFloat() + ui->lineEdit_5->text().toFloat();
    float lng1 = ui->lineEdit->text().toFloat() - ui->lineEdit_4->text().toFloat();
    float lng2 = ui->lineEdit->text().toFloat() + ui->lineEdit_4->text().toFloat();;

    my_my_sql->readData(QString("SELECT * FROM google_pano_ids WHERE  lat > '%1' AND lat < '%2' AND lng > '%3' AND lng < '%4' AND status = 1 LIMIT 1;").arg(lat1).arg(lat2).arg(lng1).arg(lng2));


    if (my_my_sql->query->next()){

        _ID = my_my_sql->query->value(my_my_sql->query->record().indexOf("pano_id")).toString();

        my_google_pano_pix->get_google_pix(_ID);

    //    qDebug()<<_ID;
        return; // if not work LIMIT 1 (0_0)
    }else {

        Pano_status("ids over");
        Enable_all_gui_element();
    }
}

void MainWindow::on_pushButton_7_clicked() //write geojson-sample.js for visualization of downloaded data
//Save data for webmap
{

    QFile datafile;
    QTextStream out;

    Disable_all_gui_element();

    datafile.setFileName("./web/geojson-sample.js");
    if (!datafile.open(QIODevice::WriteOnly | QIODevice::Truncate)){
        qDebug()<<"file not open for write"; return;}


    out.setDevice(&datafile);

    out<<"var gl_loaded = ";
    out<<"{        \n";
    out<<"    \"type\": \"FeatureCollection\",\n";
    out<<"    \"features\": [\n";


    float lat1 = ui->lineEdit_2->text().toFloat() - ui->lineEdit_5->text().toFloat();
    float lat2 = ui->lineEdit_2->text().toFloat() + ui->lineEdit_5->text().toFloat();
    float lng1 = ui->lineEdit->text().toFloat() - ui->lineEdit_4->text().toFloat();
    float lng2 = ui->lineEdit->text().toFloat() + ui->lineEdit_4->text().toFloat();;

    // my_my_sql->readData(QString("SELECT * FROM google_pano_ids WHERE lat > '%1' AND lat < '%2' AND lng > '%3' AND lng < '%4' AND status = 2;").arg(lat1).arg(lat2).arg(lng1).arg(lng2));


    my_my_sql->readData(QString("SELECT * FROM google_pano_ids WHERE status = 2"));

    int record=0;

    while(my_my_sql->query->next()){
        if (!record==0){
            out<<"        },\n";
        }
        record++;
        out<<"        {\n";
        out<<"            \"geometry\": {\n";
        out<<"                \"type\": \"Point\",\n";
        out<<"                \"coordinates\": [\n";
        out<<"                    "<<my_my_sql->query->value(my_my_sql->query->record().indexOf("original_lng")).toString()<<",\n";
        out<<"                    "<<my_my_sql->query->value(my_my_sql->query->record().indexOf("original_lat")).toString()<<"\n";
        out<<"                ]\n";
        out<<"            },\n";
        out<<"            \"type\": \"Feature\",\n";
        out<<"            \"properties\": {\n";
        out<<"                \"popupContent\": \""<<my_my_sql->query->value(my_my_sql->query->record().indexOf("pano_id")).toString()<<"\",\n";
        out<<"                \"local_path\": \""<<my_my_sql->query->value(my_my_sql->query->record().indexOf("local_path")).toString()<<"\"\n";
        out<<"            },\n";
        out<<"            \"id\": "<<QString("%1").arg(record)<<"\n";

    }

    out<<" }]};";


    out<<"\nvar gl_noloaded = ";
    out<<"{        \n";
    out<<"    \"type\": \"FeatureCollection\",\n";
    out<<"    \"features\": [\n";


    lat1 = ui->lineEdit_2->text().toFloat() - ui->lineEdit_5->text().toFloat();
    lat2 = ui->lineEdit_2->text().toFloat() + ui->lineEdit_5->text().toFloat();
    lng1 = ui->lineEdit->text().toFloat() - ui->lineEdit_4->text().toFloat();
    lng2 = ui->lineEdit->text().toFloat() + ui->lineEdit_4->text().toFloat();;

    my_my_sql->readData(QString("SELECT * FROM google_pano_ids WHERE lat > '%1' AND lat < '%2' AND lng > '%3' AND lng < '%4' AND status = 1;").arg(lat1).arg(lat2).arg(lng1).arg(lng2));

    record=0;

    while(my_my_sql->query->next()){
        if (!record==0){
            out<<"        },\n";
        }
        record++;
        out<<"        {\n";
        out<<"            \"geometry\": {\n";
        out<<"                \"type\": \"Point\",\n";
        out<<"                \"coordinates\": [\n";
        out<<"                    "<<my_my_sql->query->value(my_my_sql->query->record().indexOf("original_lng")).toString()<<",\n";
        out<<"                    "<<my_my_sql->query->value(my_my_sql->query->record().indexOf("original_lat")).toString()<<"\n";
        out<<"                ]\n";
        out<<"            },\n";
        out<<"            \"type\": \"Feature\",\n";
        out<<"            \"properties\": {\n";
        out<<"                \"popupContent\": \""<<my_my_sql->query->value(my_my_sql->query->record().indexOf("pano_id")).toString()<<"\"\n";
        out<<"            },\n";
        out<<"            \"id\": "<<QString("%1").arg(record)<<"\n";

    }

    out<<" }]};";
    qDebug()<<record;

    Enable_all_gui_element();

}

void MainWindow::on_pushButton_8_clicked()
{

    Disable_all_gui_element();
    my_google_pano_id->download_id_from_koord(ui->lineEdit_2->text(),ui->lineEdit->text(),ui->lineEdit_3->text());

}

void MainWindow::on_pushButton_9_clicked() //Re-download g panoramas that were not found
{
    Disable_all_gui_element();
    QString _ID;
    status_for_download_gpano=6;
    flag_GY=1;

    my_my_sql->readData(QString("SELECT * FROM google_pano_ids WHERE  status = 5 LIMIT 1;"));

    if (my_my_sql->query->next()){

        _ID = my_my_sql->query->value(my_my_sql->query->record().indexOf("pano_id")).toString();
        my_google_pano_pix->get_google_pix(_ID);

        return; // if not work LIMIT 1 (0_0)
    }else {

        Pano_status("ids over");
        Enable_all_gui_element();
    }


  //  my_google_pano_id->download_oid_where_status_zero();
}

void MainWindow::on_pushButton_5_clicked() //Re-download Ya panoramas
{

    Disable_all_gui_element();
    QString _ID;
    int ya_height;
    status_for_download_gpano=6;

    flag_GY=2;

    my_my_sql->readData(QString("SELECT * FROM yandex_pano_oids WHERE  status = 5 LIMIT 1;"));

    if (my_my_sql->query->next()){

        _ID = my_my_sql->query->value(my_my_sql->query->record().indexOf("oid")).toString();
        ya_height = my_my_sql->query->value(my_my_sql->query->record().indexOf("Zoom_height")).toInt();
        my_yandex_pano_pix->get_yandex_pix(_ID,21,7,ya_height);
    //    my_google_pano_pix->get_google_pix(_ID);

        return; // if not work LIMIT 1 (0_0)
    }else {

        Pano_status("ids over");
        Enable_all_gui_element();
    }

}

void MainWindow::reconfigure(){


    readSettings();


//    QString qs = my_my_sql->db.connectionName(); //QT magic





    SQL_init();


}

void MainWindow::on_pushButton_10_clicked()
{
            _ConfigDialog = new ConfigDialog(this);
             connect(_ConfigDialog, SIGNAL(reconfigure()),this, SLOT(reconfigure()));
            _ConfigDialog->show();

}
