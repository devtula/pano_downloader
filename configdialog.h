#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget *parent = 0);
    ~ConfigDialog();

private slots:
    void on_pushButton_clicked();


    void on_buttonBox_accepted();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

signals:
    void reconfigure();

private:
    Ui::ConfigDialog *ui;


};

#endif // CONFIGDIALOG_H
