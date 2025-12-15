#include "environment.h"

Environment::Environment() {}

Environment::Environment(int id, QString &name, QUuid uid, bool active)
{
    m_id = id;
    m_name = name;
    m_uid = uid;
    m_active = active;
}

void Environment::setName(QString &name)
{
    m_name = name;
}

void Environment::setActive(bool isActive)
{
    m_active = isActive;
}

void Environment::setId(int id)
{
    m_id = id;
}

void Environment::setUid(QUuid uid)
{
    m_uid = uid;
}

void Environment::addParam(ParamValue &paramValue)
{
    m_params.append(paramValue);
}

void Environment::setDeletedParams(QList<int> &deletedParams)
{
    m_deletedParams = deletedParams;
}

QMap<QString, QString> Environment::getAllValues() const
{
    QMap<QString, QString> result;
    for (const ParamValue &paramVal : m_params)
    {
        if (paramVal.hasValue("name") && paramVal.hasValue("value"))
        {
            if (!result.contains(paramVal.value("name")))
            {
                result.insert(paramVal.value("name"), paramVal.value("value"));
            }
        }
    }

    return result;
}

QString &Environment::name()
{
    return m_name;
}

std::optional<int> Environment::id()
{
    return m_id;
}

QUuid Environment::uid()
{
    return m_uid;
}

bool Environment::active()
{
    return m_active;
}

QList<ParamValue> &Environment::params()
{
    return m_params;
}

QList<int> &Environment::deletedParams()
{
    return m_deletedParams;
}
