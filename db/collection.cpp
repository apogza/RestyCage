#include "collection.h"

Collection::Collection() {}

Collection::Collection(std::optional<int> id, QString name, std::optional<int> parentId)
{
    m_id = id;
    m_name = name;
    m_parentId = parentId;
}

Collection::Collection(std::optional<int> id, QString name, std::optional<int> parentId, QList<Query> queries)
    :Collection(id, name, parentId)
{
    m_queries = queries;
}

std::optional<int> Collection::id()
{
    return m_id;
}

QString Collection::name()
{
    return m_name;
}

std::optional<int> Collection::parent()
{
    return m_parentId;
}

QList<Query> &Collection::queries()
{
    return m_queries;
}

void Collection::setQueries(QList<Query> queries)
{
    m_queries = queries;
}
