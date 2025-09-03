#ifndef DB_H
#define DB_H

#include "environment.h"
#include "collection.h"
#include "query.h"
#include "query_binary_body.h"

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

    bool insertCollection(Collection &collection);
    bool updateCollection(Collection &collection);

    QList<ParamValue> getEnvParams(int envId);
    bool insertEnvParam(int envId, ParamValue &paramValue);
    bool updateEnvParam(int envId, ParamValue &paramValue);
    bool deleteEnvParams(QList<int> &idList);

    bool insertQuery(Query &query);
    bool updateQuery(Query &query);

    bool saveBasicQueryAuth(BasicQueryAuth &basicQueryAuth);
    bool insertBasicQueryAuth(BasicQueryAuth &basicQueryAuth);
    bool updateBasicQueryAuth(BasicQueryAuth &basicQueryAuth);

    bool saveBearerQueryAuth(BearerQueryAuth &bearerQueryAuth);
    bool insertBearerQueryAuth(BearerQueryAuth &bearerQueryAuth);
    bool updateBearerQueryAuth(BearerQueryAuth &bearerQueryAuth);

    bool saveQueryAuth(Query &query);


    bool saveQueryHeaders(Query &query);
    bool insertQueryHeader(int queryid, ParamValue &paramValue);
    bool updateQueryHeader(ParamValue &paramValue);
    bool deleteQueryHeaders(QList<int> &idList);

    bool saveQueryParams(Query &query);
    bool insertQueryParam(int queryId, ParamValue &paramValue);
    bool updateQueryParam(ParamValue &paramvalue);
    bool deleteQueryParams(QList<int> &idList);

    bool saveQueryBody(Query &query);

    bool saveQueryBodyFormData(Query &query);
    bool insertQueryBodyFormDataParam(int queryId, ParamValue &paramValue);
    bool updateQueryBodyFormDataParam(ParamValue &paramValue);
    bool deleteQueryBodyMultipartParams(QList<int> &idParams);

    bool saveQueryBodyEncodedFormData(Query &query);
    bool insertQueryEncodedFormData(int queryId, ParamValue &paramValue);
    bool updateQueryEncodedFormData(ParamValue &paramValue);
    bool deleteQueryEncodedForm(QList<int> &idParams);

    bool saveQueryRawBody(Query &query);
    bool insertQueryRawBody(QueryRawBody &rawBody);
    bool updateQueryRawBody(QueryRawBody &rawBody);

    bool saveQueryBinaryBody(Query &query);
    bool insertQueryBinaryBody(QueryBinaryBody &binaryBody);
    bool updateQueryBinaryBody(QueryBinaryBody &binaryBody);

    bool deleteQueryDeletedParams(Query &query);

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
    bool deleteEnv(int envId);

    bool saveCollection(Collection &collection);
    bool deleteCollection(int collectionId);

    std::optional<Query> getQuery(int queryId);
    bool saveQuery(Query &query);
    bool deleteQuery(int queryId);

};

#endif // DB_H
