#ifndef BASIC_QUERY_AUTH_H
#define BASIC_QUERY_AUTH_H

#include <QString>

class BasicQueryAuth
{
public:
    BasicQueryAuth();
    BasicQueryAuth(QString &username, QString &password);
    BasicQueryAuth(int id, int queryId, QString &username, QString &password);


    std::optional<int> id();
    QString &username();
    QString &password();
    std::optional<int> queryId();

    void setId(int id);
    void setQueryId(int queryId);
    void setUsername(QString &username);
    void setPassword(QString &password);

private:
    std::optional<int> m_id;
    std::optional<int> m_queryId;
    QString m_username;
    QString m_password;
};

#endif // BASIC_QUERY_AUTH_H
