#include "environment.h"

Environment::Environment() {}

Environment::Environment(int id, QString name, bool active)
{
    m_id = id;
    m_name = name;
    m_active = active;
}

void Environment::setName(QString name)
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

void Environment::addParam(ParamValue paramValue)
{
    m_params.append(paramValue);
}

void Environment::setDeletedParams(QList<int> deletedParams)
{
    m_deletedParams = deletedParams;
}

QString Environment::name()
{
    return m_name;
}

std::optional<int> Environment::id()
{
    return m_id;
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
