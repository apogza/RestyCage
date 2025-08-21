#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <QString>
#include <QList>
#include <QVariant>

#include "paramvalue.h"

class Environment
{
public:
    Environment();
    Environment(int id, QString name, bool active);

    void setName(QString name);
    void setActive(bool isActive);
    void setId(int id);
    void addParam(ParamValue paramValue);

    QString &name();
    QVariant &id();
    bool active();
    QList<ParamValue> &params();

private:
    QVariant m_id;
    bool m_active;
    QString m_name;
    QList<ParamValue> m_params;

};

#endif // ENVIRONMENT_H
