#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QQueue>
#include <QFileDialog>
#include <QSettings>

#include "configdialog.h"

#include "yandex_pano_pix.h"
#include "yandex_pano_oid.h"
#include "my_sql.h"
#include "google_pano_id.h"
#include "google_pano_pix.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT




public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();


    void Pano_status(QString);
    void Pano_Next_Download_Y (QString);
    void Pano_Next_Download_G (QString);

    void reconfigure();


    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();

private:
    Ui::MainWindow *ui;

    ConfigDialog *_ConfigDialog;


    yandex_pano_pix* my_yandex_pano_pix;
    yandex_pano_oid* my_yandex_pano_oid;

    google_pano_id * my_google_pano_id;
    google_pano_pix * my_google_pano_pix;

    my_sql* my_my_sql;

 //   QQueue<QString> Pano_oids_QQueue;

    void readSettings();
    void writeSettings();

    void Disable_all_gui_element();
    void Enable_all_gui_element();

    void SQL_init();

    QString pano_id_database;

    QString search_path_for_downloaded_pano ;
    QString search_path_for_downloaded_Ya_pano;

    QString path_for_downloaded_pano;
    QString path_for_downloaded_Ya_pano;

    int status_for_download_gpano; //5 for normal download; 6 for redownload

    int flag_GY; //1 Go, 2 Ya
};

#endif // MAINWINDOW_H
