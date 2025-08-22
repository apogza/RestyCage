#ifndef DB_H
#define DB_H

#include "environment.h"
#include "collection.h"
#include "query.h"

#include <QtSql/QSqlDatabase>

class Db
{
private:
    Db();
    void init();

    void initEnvs();
    void initCollections();
    void initQueries();

    bool insertEnv(Environment &environment);
    bool updateEnv(Environment &environment);
    bool deleteEnvParameters(const QList<int> &deletedParams);

    bool insertCollection(Collection &collection);
    bool deleteCollection(int collectionId);

    QList<ParamValue> getEnvParams(int envId);
    bool insertEnvParam(int envId, ParamValue &paramValue);
    bool updateEnvParam(int envId, ParamValue &paramValue);

    QSqlDatabase m_db;
public:
    Db &operator=(const Db &) = delete;
    static Db &instance();
    void close();

    QList<Collection> getCollections(bool getQueries = false);
    std::optional<Collection> getCollection(int collectionId, bool getQueries = false);
    QList<Query> getCollectionQueries(int collectionId);

    QList<Environment> getEnvs();
    std::optional<Environment> getEnv(int envId);
    bool saveEnv(Environment &environment);



};

#endif // DB_H
