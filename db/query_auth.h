#ifndef QUERY_AUTH_H
#define QUERY_AUTH_H

#include <QString>

class QueryAuth
{
public:
    QueryAuth();
    QueryAuth(QString username, QString password);
    QueryAuth(QString bearerToken);

    enum AuthType { None, Basic, BearerToken };

    QueryAuth::AuthType authType();

    QString &username();
    QString &password();
    QString &bearerToken();
    int queryId();

    void setId(int id);

    static QString authTypeToString(const AuthType authType);
    static AuthType authTypeFromString(const QString &authType);
private:
    std::optional<int> m_id;
    int m_queryId;
    QString m_username;
    QString m_password;
    QString m_bearerToken;
};

#endif // QUERY_AUTH_H
