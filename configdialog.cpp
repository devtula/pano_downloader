#include "configdialog.h"
#include "ui_configdialog.h"

ConfigDialog::ConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);
    QSettings settings("dev.tula.bz", "Pano downloader");
    ui->lineEdit->setText(settings.value("pano_id_database",QString("pano_ids.db3")).toString());
    ui->lineEdit_2->setText(settings.value("path_for_downloaded_pano",QString("./pano")).toString());
    ui->lineEdit_3->setText(settings.value("path_for_downloaded_Ya_pano",QString("./pano")).toString());


}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::on_pushButton_clicked()
{

    QString pano_id_database = QFileDialog::getSaveFileName(this, tr("Open pano_id_database"),
                                                            ui->lineEdit->text(),"*"
                                                            ,0,QFileDialog::DontConfirmOverwrite
                                                            );

    if (!pano_id_database.isEmpty()){
        QFile pano_id_database_file(pano_id_database);

        if (pano_id_database_file.open(QIODevice::WriteOnly |  QIODevice::Append)){
            ui->lineEdit->setText(pano_id_database);
            QSettings settings("dev.tula.bz", "Pano downloader");

            settings.setValue("pano_id_database",pano_id_database );
            emit reconfigure();
        }else {
            QMessageBox(QMessageBox::Warning,"ERROR",QString("Cannot open file %1 for writing").append(pano_id_database));
        }
        pano_id_database_file.close();
    }
}


void ConfigDialog::on_pushButton_2_clicked()
{
    QSettings settings("dev.tula.bz", "Pano downloader");
    QString path_for_downloaded_pano = QFileDialog::getExistingDirectory(this, tr("Open folder for saveing G pano"),
                                                                         ui->lineEdit_2->text());
    if (!path_for_downloaded_pano.isEmpty()){
        QFile test_file_for_path_for_downloaded_pano (QString("%1/.testwriting").arg(path_for_downloaded_pano));

        if (test_file_for_path_for_downloaded_pano.open(QIODevice::WriteOnly |  QIODevice::Append)){
            ui->lineEdit_2->setText(path_for_downloaded_pano);
            settings.setValue("path_for_downloaded_pano",path_for_downloaded_pano );
            test_file_for_path_for_downloaded_pano.remove();
            emit reconfigure();
        }else {
            QMessageBox(QMessageBox::Warning,"ERROR",QString("Cannot open folder %1 for writing").append(path_for_downloaded_pano));
        }

    }else {
        QMessageBox(QMessageBox::Warning,"ERROR",QString("folder for save G pano not set!"));
    }


}

void ConfigDialog::on_pushButton_3_clicked()
{
    QSettings settings("dev.tula.bz", "Pano downloader");
    QString path_for_downloaded_Ya_pano = QFileDialog::getExistingDirectory(this, tr("Open folder for saveing Ya pano"),
                                                                            ui->lineEdit_3->text());
    if (!path_for_downloaded_Ya_pano.isEmpty()){
        QFile test_file_for_path_for_downloaded_Ya_pano (QString("%1/.testwriting").arg(path_for_downloaded_Ya_pano));

        if (test_file_for_path_for_downloaded_Ya_pano.open(QIODevice::WriteOnly |  QIODevice::Append)){
            ui->lineEdit_3->setText(path_for_downloaded_Ya_pano);
            settings.setValue("path_for_downloaded_Ya_pano",path_for_downloaded_Ya_pano );
            test_file_for_path_for_downloaded_Ya_pano.remove();
            emit reconfigure();
        }else {
            QMessageBox(QMessageBox::Warning,"ERROR",QString("Cannot open folder %1 for writing").append(path_for_downloaded_Ya_pano));
        }

    }else {
        QMessageBox(QMessageBox::Warning,"ERROR",QString("folder for save Ya pano not set!"));
    }

}

void ConfigDialog::on_buttonBox_accepted()
{
    QSettings settings("dev.tula.bz", "Pano downloader");

    QString pano_id_database=ui->lineEdit->text();

    if (!pano_id_database.isEmpty()){
        QFile pano_id_database_file(pano_id_database);

        if (pano_id_database_file.open(QIODevice::WriteOnly |  QIODevice::Append)){
            ui->lineEdit->setText(pano_id_database);
            settings.setValue("pano_id_database",pano_id_database );
            emit reconfigure();
        }else {
            QMessageBox(QMessageBox::Warning,"ERROR",QString("Cannot open file %1 for writing").append(pano_id_database));
        }
        pano_id_database_file.close();
    }else {
        QMessageBox(QMessageBox::Warning,"ERROR",QString("File name database not set!"));
    }

    QString path_for_downloaded_pano = ui->lineEdit_2->text();
    if (!path_for_downloaded_pano.isEmpty()){
        QFile test_file_for_path_for_downloaded_pano (QString("%1/.testwriting").arg(path_for_downloaded_pano));

        if (test_file_for_path_for_downloaded_pano.open(QIODevice::WriteOnly |  QIODevice::Append)){
            ui->lineEdit_2->setText(path_for_downloaded_pano);
            settings.setValue("path_for_downloaded_pano",path_for_downloaded_pano );
            test_file_for_path_for_downloaded_pano.remove();
            emit reconfigure();
        }else {
            QMessageBox(QMessageBox::Warning,"ERROR",QString("Cannot open folder %1 for writing").append(path_for_downloaded_pano));
        }

    }else {
        QMessageBox(QMessageBox::Warning,"ERROR",QString("folder for save G pano not set!"));
    }

    QString path_for_downloaded_Ya_pano = ui->lineEdit_3->text();
    if (!path_for_downloaded_Ya_pano.isEmpty()){
        QFile test_file_for_path_for_downloaded_Ya_pano (QString("%1/.testwriting").arg(path_for_downloaded_Ya_pano));

        if (test_file_for_path_for_downloaded_Ya_pano.open(QIODevice::WriteOnly |  QIODevice::Append)){
            ui->lineEdit_3->setText(path_for_downloaded_Ya_pano);
            settings.setValue("path_for_downloaded_Ya_pano",path_for_downloaded_Ya_pano );
            test_file_for_path_for_downloaded_Ya_pano.remove();
            emit reconfigure();
        }else {
            QMessageBox(QMessageBox::Warning,"ERROR",QString("Cannot open folder %1 for writing").append(path_for_downloaded_Ya_pano));
        }

    }else {
        QMessageBox(QMessageBox::Warning,"ERROR",QString("folder for save Ya pano not set!"));
    }

}
