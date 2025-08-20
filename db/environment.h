#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <QString>
#include <QList>

#include "paramvalue.h"

class Environment
{
public:
    Environment();
    void setName(QString name);
    void addParam(ParamValue paramValue);
    QList<ParamValue> &getParams();


private:
    QString name;
    QList<ParamValue> params;

};

#endif // ENVIRONMENT_H
