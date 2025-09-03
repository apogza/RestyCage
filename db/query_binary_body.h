#ifndef QUERY_BINARY_BODY_H
#define QUERY_BINARY_BODY_H
#include <QString>

class QueryBinaryBody
{
public:
    QueryBinaryBody();
    QueryBinaryBody(QString &filepath);
    QueryBinaryBody(int id, QString &filepath);
    QueryBinaryBody(int queryId, int id, QString &filepath);

    std::optional<int> id();
    std::optional<int> queryId();
    QString &filepath();

    void setId(int id);
    void setQueryId(int id);
    void setFilepath(QString &filepath);

private:
    std::optional<int> m_id;
    std::optional<int> m_queryId;
    QString m_filepath;

};

#endif // QUERY_BINARY_BODY_H
