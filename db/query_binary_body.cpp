#include "query_binary_body.h"

QueryBinaryBody::QueryBinaryBody() {}

QueryBinaryBody::QueryBinaryBody(QString &filepath)
{
    m_filepath = filepath;
}

QueryBinaryBody::QueryBinaryBody(int id, QString &filepath)
    : QueryBinaryBody(filepath)
{
    m_id = id;
}

QueryBinaryBody::QueryBinaryBody(int queryId, int id, QString &filepath)
    : QueryBinaryBody(id, filepath)
{
    m_queryId = queryId;
}

std::optional<int> QueryBinaryBody::id()
{
    return m_id;
}

std::optional<int> QueryBinaryBody::queryId()
{
    return m_queryId;
}

QString &QueryBinaryBody::filepath()
{
    return m_filepath;
}

void QueryBinaryBody::setId(int id)
{
    m_id = id;
}

void QueryBinaryBody::setQueryId(int id)
{
    m_queryId = id;
}
