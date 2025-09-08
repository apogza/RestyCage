#ifndef BEARER_QUERY_AUTH_H
#define BEARER_QUERY_AUTH_H

#include <QString>

class BearerQueryAuth
{
public:
    BearerQueryAuth();
    BearerQueryAuth(QString &bearerToken);
    BearerQueryAuth(int id, QString &bearerToken);
    BearerQueryAuth(int id, int queryId, QString &bearerToken);

    std::optional<int> id();
    std::optional<int> queryId();
    QString &bearerToken();

    void setId(int id);
    void setQueryId(int queryId);

private:
    std::optional<int> m_id;
    std::optional<int> m_queryId;
    QString m_bearerToken;

};

#endif // BEARER_QUERY_AUTH_H
