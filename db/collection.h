#ifndef COLLECTION_H
#define COLLECTION_H

#include "query.h"

#include <QString>

class Collection
{
public:
    Collection();
    Collection (QString name, std::optional<int> parentId);
    Collection (std::optional<int> id, QString name, std::optional<int> parentId);
    Collection (std::optional<int> id, QString name, std::optional<int> parentId, QList<Query> queries);

    std::optional<int> id();
    void setId(std::optional<int> id);

    QString name();
    std::optional<int> parent();
    QList<Query>& queries();
    void setQueries(QList<Query> queries);


private:
    std::optional<int> m_id;
    QString m_name;
    std::optional<int> m_parentId;
    QList<Query> m_queries;
};

#endif // COLLECTION_H
