#ifndef BEARER_QUERY_AUTH_H
#define BEARER_QUERY_AUTH_H

#include <QString>

class BearerQueryAuth
{
public:
    BearerQueryAuth();
    BearerQueryAuth(QString &bearerToken);
    BearerQueryAuth(int id, QString &bearerToken);

    std::optional<int> id();
    QString &bearerToken();

    void setId(int id);

private:
    std::optional<int> m_id;
    QString m_bearerToken;

};

#endif // BEARER_QUERY_AUTH_H
