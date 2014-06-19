#include "my_panoparam.h"

my_PanoParam::my_PanoParam(QObject *parent) :
    QObject(parent)
{
         i=0;j=0;z=0;xmax=0;ymax=0;oui="NONE"; URL="", flag=0;

}

my_PanoParam& my_PanoParam::operator=(const my_PanoParam& _my_PanoParam)
{

    i=_my_PanoParam.i;
    j=_my_PanoParam.j;
    z=_my_PanoParam.z;

    xmax=_my_PanoParam.xmax;
    ymax=_my_PanoParam.ymax;

    oui=_my_PanoParam.oui;
    URL=_my_PanoParam.URL;

    flag = _my_PanoParam.flag;

    return *this;
}

my_PanoParam::my_PanoParam(const my_PanoParam & _my_PanoParam){

    i=_my_PanoParam.i;
    j=_my_PanoParam.j;
    z=_my_PanoParam.z;

    xmax=_my_PanoParam.xmax;
    ymax=_my_PanoParam.ymax;

    oui=_my_PanoParam.oui;
    URL=_my_PanoParam.URL;

    flag = _my_PanoParam.flag;
}

void my_PanoParam::Copy(const my_PanoParam _my_PanoParam){

    i=_my_PanoParam.i;
    j=_my_PanoParam.j;
    z=_my_PanoParam.z;

    xmax=_my_PanoParam.xmax;
    ymax=_my_PanoParam.ymax;

    oui=_my_PanoParam.oui;
    URL=_my_PanoParam.URL;

    flag = _my_PanoParam.flag;

}
