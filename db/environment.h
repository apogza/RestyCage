#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <QString>
#include <QList>
#include <QVariant>
#include <QUuid>

#include "param_value.h"

class Environment
{
public:
    Environment();
    Environment(int id, QString &name, QUuid uid, bool active);

    void setName(QString &name);
    void setActive(bool isActive);
    void setId(int id);
    void setUid(QUuid uid);
    void addParam(ParamValue &paramValue);
    void setDeletedParams(QList<int> &deletedParams);

    QMap<QString, QString> getAllValues() const;

    QString &name();
    std::optional<int> id();
    QUuid uid();
    bool active();
    QList<ParamValue> &params();
    QList<int> &deletedParams();

private:
    std::optional<int> m_id;
    QUuid m_uid = QUuid::createUuid();

    bool m_active;
    QString m_name;
    QList<ParamValue> m_params;
    QList<int> m_deletedParams;


};

#endif // ENVIRONMENT_H
