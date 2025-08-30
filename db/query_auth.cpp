#include "query_auth.h"

QueryAuth::QueryAuth()
{
}

QueryAuth::QueryAuth(QString username, QString password)
{
    m_username = username;
    m_password = password;
}

QueryAuth::QueryAuth(QString bearerToken)
{
    m_bearerToken = bearerToken;
}

QString QueryAuth::authTypeToString(const AuthType authType)
{
    QString result;

    switch (authType) {
    case AuthType::None:
        result = "None";
        break;
    case AuthType::Basic:
        result = "Basic";
        break;
    case AuthType::BearerToken:
        result = "Bearer";
        break;
    default:
        break;
    }

    return result;
}

QueryAuth::AuthType QueryAuth::authTypeFromString(const QString &authType)
{
    if (authType == "Bearer")
    {
        return AuthType::BearerToken;
    }
    else if (authType == "Basic")
    {
        return AuthType::Basic;
    }
    else
    {
        return AuthType::None;
    }
}

QueryAuth::AuthType QueryAuth::authType()
{
    if (!m_username.isNull() && !m_username.isEmpty()
        && !m_password.isNull() && !m_password.isEmpty())
    {
        return AuthType::Basic;
    }


    if (!m_bearerToken.isNull() && !m_bearerToken.isEmpty())
    {
        return AuthType::Basic;
    }

    return AuthType::None;
}

std::optional<int> &QueryAuth::id()
{
    return m_id;
}

QString &QueryAuth::username()
{
    return m_username;
}

QString &QueryAuth::password()
{
    return m_password;
}

QString &QueryAuth::bearerToken()
{
    return m_bearerToken;
}

int QueryAuth::queryId()
{
    return m_queryId;
}

void QueryAuth::setId(int id)
{
    m_id = id;
}


