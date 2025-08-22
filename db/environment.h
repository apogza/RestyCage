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
    void setDeletedParams(QList<int> deletedParams);

    QString name();
    std::optional<int> id();
    bool active();
    QList<ParamValue> &params();
    QList<int> &deletedParams();

private:
    std::optional<int> m_id;
    bool m_active;
    QString m_name;
    QList<ParamValue> m_params;
    QList<int> m_deletedParams;

};

#endif // ENVIRONMENT_H
