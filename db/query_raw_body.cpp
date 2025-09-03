#include "query_raw_body.h"

QueryRawBody::QueryRawBody() {}

QueryRawBody::QueryRawBody(RawBodyType rawBodyType, QString &value)
{
    m_rawBodyType = rawBodyType;
}

QueryRawBody::QueryRawBody(int id, int queryId, RawBodyType rawBodyType, QString &value)
    :QueryRawBody(rawBodyType, value)
{
    m_id = id;
    m_queryId = queryId;
}

QString QueryRawBody::rawBodyToString(const RawBodyType rawBodyType)
{
    QString result;

    switch (rawBodyType){
    case RawBodyType::JSON:
        result = "JSON";
        break;
    case RawBodyType::Plain:
        result = "Plain";
        break;

    case RawBodyType::XML:
        result = "XML";
        break;
    case RawBodyType::HTML:
        result = "HTML";
        break;
    case RawBodyType::JavaScript:
        result = "JavaScript";
        break;
    }

    return result;
}

std::optional<int> QueryRawBody::id()
{
    return m_id;
}

std::optional<int> QueryRawBody::queryId()
{
    return m_queryId;
}

QString &QueryRawBody::value()
{
    return m_value;
}

void QueryRawBody::setId(int id)
{
    m_id = id;
}

QueryRawBody::RawBodyType QueryRawBody::rawBodyType()
{
    return m_rawBodyType;
}

QueryRawBody::RawBodyType QueryRawBody::rawBodyTypeFromString(const QString &rawBodyType)
{
    if (rawBodyType == "JSON")
    {
        return RawBodyType::JSON;
    }
    else if (rawBodyType == "Plain")
    {
        return RawBodyType::Plain;
    }
    else if (rawBodyType == "XML")
    {
        return RawBodyType::XML;
    }
    else if (rawBodyType == "HTML")
    {
        return RawBodyType::HTML;
    }
    else
    {
        return RawBodyType::JavaScript;
    }
}
