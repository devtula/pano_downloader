#ifndef MY_PANOPARAM_H
#define MY_PANOPARAM_H

#include <QObject>


class my_PanoParam : public QObject
{
    Q_OBJECT
public:
    explicit my_PanoParam(QObject *parent = 0);

    my_PanoParam& operator=(const my_PanoParam& );
    my_PanoParam(const my_PanoParam &);
    void Copy(const my_PanoParam);

public:

    int i;
    int j;
    int z;

    int xmax;
    int ymax;

    QString URL;
    QString oui;

    int flag; // 0 for normal download, 1 for ya first start get oid

signals:

public slots:

};




#endif // MY_PANOPARAM_H
