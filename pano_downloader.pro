#-------------------------------------------------
#
# Project created by QtCreator 2014-03-11T17:19:40
#
#-------------------------------------------------

QT       += core gui network xml sql# webkit webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pano_downloader
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        yandex_pano_pix.cpp \
        http_downloader.cpp \
        my_panoparam.cpp \
    yandex_pano_oid.cpp \
    my_sql.cpp \
    google_pano_id.cpp \
    google_pano_pix.cpp \
    configdialog.cpp

HEADERS  += mainwindow.h \
         yandex_pano_pix.h \
         http_downloader.h \
         my_panoparam.h \
    yandex_pano_oid.h \
    my_sql.h \
    google_pano_id.h \
    google_pano_pix.h \
    configdialog.h

FORMS    += mainwindow.ui \
    configdialog.ui
