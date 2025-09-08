#include "basic_query_auth.h"

BasicQueryAuth::BasicQueryAuth()
{
}

BasicQueryAuth::BasicQueryAuth(QString &username, QString &password)
{
    m_username = username;
    m_password = password;
}

BasicQueryAuth::BasicQueryAuth(int id, int queryId, QString &username, QString &password)
    : BasicQueryAuth(username, password)
{
    m_id = id;
    m_queryId = queryId;
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

std::optional<int> BasicQueryAuth::queryId()
{
    return m_queryId;
}

void BasicQueryAuth::setId(int id)
{
    m_id = id;
}

void BasicQueryAuth::setQueryId(int queryId)
{
    m_queryId = queryId;
}

void BasicQueryAuth::setUsername(QString &username)
{
    m_username = username;
}

void BasicQueryAuth::setPassword(QString &password)
{
    m_password = password;
}


