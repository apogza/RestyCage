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

std::optional<int> BearerQueryAuth::id()
{
    return m_id;
}

QString &BearerQueryAuth::bearerToken()
{
    return m_bearerToken;
}

void BearerQueryAuth::setId(int id)
{
    m_id = id;
}
