#ifndef QUERY_RAW_BODY_H
#define QUERY_RAW_BODY_H

#include <QString>

class QueryRawBody
{


public:
    enum RawBodyType { JSON, Plain, XML, HTML, JavaScript };

    QueryRawBody();
    QueryRawBody(RawBodyType rawBodyType, QString &value);
    QueryRawBody(int id, int queryId, RawBodyType rawBodyType, QString &value);

    static QString rawBodyToString(const RawBodyType rawBodyType);
    static RawBodyType rawBodyTypeFromString(const QString &rawBodyType);

    std::optional<int> id();
    std::optional<int> queryId();
    RawBodyType rawBodyType();
    QString &value();

    void setId(int id);
    void setQueryId(int queryId);

private:
    std::optional<int> m_id;
    std::optional<int> m_queryId;

    RawBodyType m_rawBodyType;
    QString m_value;
};

#endif // QUERY_RAW_BODY_H
