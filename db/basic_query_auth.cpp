#include "basic_query_auth.h"

BasicQueryAuth::BasicQueryAuth()
{
}

BasicQueryAuth::BasicQueryAuth(QString username, QString password)
{
    m_username = username;
    m_password = password;
}

std::optional<int> BasicQueryAuth::id()
{
    return m_id;
}

QString &BasicQueryAuth::username()
{
    return m_username;
}

QString &BasicQueryAuth::password()
{
    return m_password;
}

int BasicQueryAuth::queryId()
{
    return m_queryId;
}

void BasicQueryAuth::setId(int id)
{
    m_id = id;
}


