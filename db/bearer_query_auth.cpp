#include "bearer_query_auth.h"

BearerQueryAuth::BearerQueryAuth()
{
}

BearerQueryAuth::BearerQueryAuth(QString &bearerToken)
{
    m_bearerToken = bearerToken;
}

BearerQueryAuth::BearerQueryAuth(int id, QString &bearerToken)
    :BearerQueryAuth(bearerToken)
{
    m_id = id;
}

BearerQueryAuth::BearerQueryAuth(int id, int queryId, QString &bearerToken)
{
    m_id = id;
    m_queryId = queryId;
    m_bearerToken = bearerToken;
}

std::optional<int> BearerQueryAuth::id()
{
    return m_id;
}

std::optional<int> BearerQueryAuth::queryId()
{
    return m_queryId;
}

QString &BearerQueryAuth::bearerToken()
{
    return m_bearerToken;
}

void BearerQueryAuth::setId(int id)
{
    m_id = id;
}

void BearerQueryAuth::setQueryId(int queryId)
{
    m_queryId = queryId;
}
