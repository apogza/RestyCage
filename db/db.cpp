#include "db.h"
#include "environment.h"
#include "../constants.h"

#include <QDebug>
#include <QStringList>
#include <QSqlQuery>
#include <QSqlError>

Db::Db()
{
    init();
    initEnvs();
    initCollections();
    initQueries();
}

void Db::init()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbPath);

    m_db.open();

    for (QString &table: m_db.tables())
    {
        qDebug() << table;
    }

    QSqlQuery pragmaForeignKeys(m_db);
    pragmaForeignKeys.exec("PRAGMA foreign_keys = ON;");
}

void Db::initEnvs()
{
    QStringList tables = m_db.tables();

    if (!tables.contains(envsTable))
    {
        QSqlQuery createEnvsTable(m_db);
        bool execResult = createEnvsTable.exec(
            "CREATE TABLE envs("
            "id INTEGER PRIMARY KEY,"
            "name TEXT NOT NULL,"
            "active INTEGER);");

        if (execResult)
        {
            qDebug() << "Created envs table";
        }
    }

    if (!tables.contains(envsParamsTable))
    {
        QSqlQuery createEnvsParamsTable(m_db);
        bool execResult = createEnvsParamsTable.exec(
            "CREATE TABLE envs_params("
            "id INTEGER PRIMARY KEY,"
            "env_id INTEGER,"
            "name TEXT,"
            "value TEXT,"
            "description TEXT,"
            "FOREIGN KEY (env_id) REFERENCES envs on DELETE CASCADE ON UPDATE NO ACTION);");

        if (execResult)
        {
            qDebug() << "Created envs parameter table";
        }
    }
}

void Db::initCollections()
{
    QStringList tables = m_db.tables();

    if (!tables.contains(collectionsTable))
    {
        QSqlQuery createCollectionsTable(m_db);
        bool execResult = createCollectionsTable.exec(
            "CREATE TABLE collections("
            "id INTEGER PRIMARY KEY,"
            "parent_id INTEGER,"
            "name TEXT,"
            "FOREIGN KEY (parent_id) REFERENCES collections(id) ON DELETE CASCADE ON UPDATE NO ACTION);");

        if (execResult)
        {
            qDebug() << "Created collections table";
        }
    }
}

void Db::initQueries()
{
    QStringList tables = m_db.tables();

    if (!tables.contains(queriesTable))
    {
        QSqlQuery createQueriesTable(m_db);
        bool execResult = createQueriesTable.exec(
            "CREATE TABLE queries("
            "id INTEGER PRIMARY KEY,"
            "collection_id INTEGER,"
            "name TEXT,"
            "method TEXT,"
            "url TEXT,"
            "auth_type INTEGER,"
            "body_type INTEGER,"
            "FOREIGN KEY (collection_id) REFERENCES collections(id) ON DELETE CASCADE ON UPDATE NO ACTION);");

        if (execResult)
        {
            qDebug() << "Created queries table";
        }
    }

    if (!tables.contains(queriesParamsTable))
    {
        QSqlQuery createQueriesParamsTable(m_db);
        bool execResult = createQueriesParamsTable.exec(
            "CREATE TABLE queries_params("
            "id INTEGER PRIMARY KEY,"
            "query_id INTEGER,"
            "name TEXT,"
            "value TEXT,"
            "description TEXT,"
            "FOREIGN KEY (query_id) REFERENCES queries(id) ON DELETE CASCADE ON UPDATE NO ACTION);");

        if (execResult)
        {
            qDebug() << "Create queries params table";
        }
    }

    if (!tables.contains(queriesHeadersTable))
    {
        QSqlQuery createQueriesHeadersTable(m_db);

        bool execResult = createQueriesHeadersTable.exec(
            "CREATE TABLE queries_headers("
            "id INTEGER PRIMARY KEY,"
            "query_id INTEGER,"
            "name TEXT,"
            "value TEXT,"
            "description TEXT,"
            "FOREIGN KEY (query_id) REFERENCES queries(id) ON DELETE CASCADE ON UPDATE NO ACTION);");

        if (execResult)
        {
            qDebug() << "Create queries headers table";
        }
    }

    if (!tables.contains(queriesBasicAuthTable))
    {
        QSqlQuery createQueriesBasicAuthTable(m_db);
        bool execResult = createQueriesBasicAuthTable.exec(
            "CREATE TABLE queries_auth_basic("
            "id INTEGER PRIMARY KEY,"
            "query_id INTEGER,"            
            "username TEXT,"
            "password TEXT,"            
            "FOREIGN KEY (query_id) REFERENCES queries(id) ON DELETE CASCADE ON UPDATE NO ACTION);");

        if (execResult)
        {
            qDebug() << "Create queries basic auth table";
        }
    }

    if (!tables.contains(queriesBearerAuthTable))
    {
        QSqlQuery createQueriesBearerAuthTable(m_db);

        bool execResult = createQueriesBearerAuthTable.exec(
            "CREATE TABLE queries_auth_bearer("
            "id INTEGER PRIMARY KEY,"
            "query_id INTEGER,"
            "bearer_token TEXT,"
            "FOREIGN KEY (query_id) REFERENCES queries(id) ON DELETE CASCADE ON UPDATE NO ACTION);");

        if (execResult)
        {
            qDebug() << "Create queries bearer auth table";
        }
    }

    if (!tables.contains(queriesFormDataBodyTable))
    {
        QSqlQuery createQueriesFormDatabodyTable(m_db);
        bool execResult = createQueriesFormDatabodyTable.exec(
            "CREATE TABLE queries_form_data_body("
            "id INTEGER PRIMARY KEY,"
            "query_id INTEGER,"
            "name TEXT,"
            "type INTEGER,"
            "value TEXT,"
            "description TEXT,"
            "FOREIGN KEY (query_id) REFERENCES queries(id) ON DELETE CASCADE ON UPDATE NO ACTION);");

        if (execResult)
        {
            qDebug() << "Create queries form data body table";
        }
    }

    if (!tables.contains(queriesEncodedFormBodyTable))
    {
        QSqlQuery createQueriesEncodedFormBody(m_db);
        bool execResult = createQueriesEncodedFormBody.exec(
            "CREATE TABLE queries_encoded_form_body("
            "id INTEGER PRIMARY KEY,"
            "query_id INTEGER,"
            "name TEXT,"
            "value TEXT,"
            "description TEXT,"
            "FOREIGN KEY(query_id) REFERENCES queries(id) ON DELETE CASCADE ON UPDATE NO ACTION);");

        if (execResult)
        {
            qDebug() << "Create queries encode form data body table";
        }
    }

    if (!tables.contains(queriesRawBodyTable))
    {
        QSqlQuery createQueriesRawBody(m_db);
        bool execResult = createQueriesRawBody.exec(
            "CREATE TABLE queries_raw_body("
            "id INTEGER PRIMARY KEY,"
            "query_id INTEGER,"
            "type INTEGER,"
            "value TEXT,"
            "FOREIGN KEY (query_id) REFERENCES queries(id) ON DELETE CASCADE ON UPDATE NO ACTION);");
        if (execResult)
        {
            qDebug() << "Create queries raw body table";
        }
    }

    if (!tables.contains(queriesBinaryBodyTable))
    {
        QSqlQuery createQueriesBinaryBody(m_db);
        bool execResult = createQueriesBinaryBody.exec(
            "CREATE TABLE queries_binary_body("
            "id INTEGER PRIMARY KEY,"
            "query_id INTEGER,"
            "file_path TEXT,"
            "FOREIGN KEY (query_id) REFERENCES queries(id) ON DELETE CASCADE ON UPDATE NO ACTION);");

        if (execResult)
        {
            qDebug() << "Create queries binary body table";
        }
    }
}

bool Db::saveEnv(Environment &env)
{
    std::optional<int> envId = env.id();
    bool result = true;

    if (envId.has_value())
    {
        result = updateEnv(env);
    }
    else
    {
        result = insertEnv(env);
    }

    result = result && deleteEnvParams(env.deletedParams());

    return result;
}

bool Db::deleteEnv(int envId)
{
    QSqlQuery deleteEnvQuery(m_db);
    deleteEnvQuery.prepare("DELETE from envs WHERE id = :env_id");
    deleteEnvQuery.bindValue(":env_id", envId);

    return deleteEnvQuery.exec();
}

bool Db::saveCollection(Collection &collection)
{
    if (collection.id().has_value())
    {
        return updateCollection(collection);
    }
    else
    {
        return insertCollection(collection);
    }
}

Db &Db::instance()
{
    static Db db;

    return db;
}

void Db::close()
{
    if (m_db.isValid())
    {
        m_db.close();
    }
}

QList<Collection> Db::getCollections(bool getQueries)
{
    QList<Collection> collections;

    QSqlQuery collectionsQuery("SELECT id, name, parent_id from collections ORDER BY parent_id;", m_db);

    while (collectionsQuery.next())
    {
        int id = collectionsQuery.value(0).toInt();
        QString name = collectionsQuery.value(1).toString();
        QVariant parentIdDbValue = collectionsQuery.value(2);

        std::optional<int> parentId;
        if (!parentIdDbValue.isNull())
        {
            parentId = parentIdDbValue.toInt();
        }

        if (getQueries)
        {
            QList<Query> queries = getCollectionQueries(id);
            collections.append({id, name, parentId, queries});
        }
        else
        {
            collections.append({id, name, parentId});
        }
    }

    return collections;
}

std::optional<Collection> Db::getCollection(int collectionId, bool getQueries)
{
    Collection collection;

    QSqlQuery collectionQuery(m_db);
    collectionQuery.prepare("SELECT id, name, parent_id from collections WHERE id = :collection_id;");
    collectionQuery.bindValue(":collection_id", collectionId);

    if (collectionQuery.exec() && collectionQuery.next())
    {
        int id = collectionQuery.value(0).toInt();
        QString name = collectionQuery.value(1).toString();

        std::optional<int> parentId;
        QVariant parentIdDbValue = collectionQuery.value(2).toString();

        if (!parentIdDbValue.isNull())
        {
            parentId = parentIdDbValue.toInt();
        }

        if (getQueries)
        {
            QList<Query> queries = getCollectionQueries(id);
            return Collection(id, name, parentId, queries);
        }
        else
        {
            return Collection(id, name, parentId);
        }
    }

    return std::nullopt;
}

QList<Query> Db::getCollectionQueries(int collectionId)
{
    QSqlQuery collectionsQueries(m_db);
    collectionsQueries.prepare("SELECT id, name from queries WHERE collection_id = :collection_id;");

    collectionsQueries.bindValue(":collection_id", collectionId);
    collectionsQueries.exec();

    QList<Query> result;

    while(collectionsQueries.next())
    {
        int id = collectionsQueries.value(0).toInt();
        QString name = collectionsQueries.value(1).toString();

        result.append({id, name});
    }

    return result;
}

QList<Environment> Db::getEnvs()
{
    QList<Environment> result;

    QSqlQuery envsQuery("SELECT id, name, active from envs;", m_db);

    while (envsQuery.next())
    {
        int id = envsQuery.value(0).toInt();
        QString name = envsQuery.value(1).toString();
        bool active = envsQuery.value(2).toBool();

        result.append({id, name, active});
    }

    return result;
}

std::optional<Environment> Db::getEnv(int envId)
{
    QSqlQuery envQuery(m_db);
    envQuery.prepare("SELECT id, name, active from envs WHERE id = :env_id;");
    envQuery.bindValue(":env_id", envId);

    if (envQuery.exec() && envQuery.next())
    {
        int id = envQuery.value(0).toInt();
        QString name = envQuery.value(1).toString();
        bool active = envQuery.value(2).toBool();

        Environment env(id, name, active);

        QList<ParamValue> envParams = getEnvParams(id);
        for (ParamValue &param: envParams)
        {
            env.addParam(param);
        }

        return env;
    }

    return std::nullopt;
}

bool Db::insertEnv(Environment &environment)
{
    QSqlQuery insertEnv(m_db);
    insertEnv.prepare("INSERT INTO envs(name, active) VALUES(:name, :active);");

    int activeVal = environment.active() ? 1 : 0;

    insertEnv.bindValue(":name", environment.name());
    insertEnv.bindValue(":active", activeVal);

    bool result = insertEnv.exec();

    if (!result)
    {
        return false;
    }

    int envId = insertEnv.lastInsertId().toInt();
    environment.setId(envId);

    QList<ParamValue> &params = environment.params();

    for (ParamValue &param: params)
    {
        result = result && insertEnvParam(envId, param);
    }

    return result;
}

bool Db::updateEnv(Environment &environment)
{
    QSqlQuery updateEnv(m_db);

    updateEnv.prepare("UPDATE envs SET name = :name, active = :active WHERE id = :id;");

    updateEnv.bindValue(":name", environment.name());
    updateEnv.bindValue(":active", environment.active());
    updateEnv.bindValue(":id", environment.id().value());

    bool result = updateEnv.exec();

    QList<ParamValue> &params = environment.params();
    int envId = environment.id().value();

    for (ParamValue &param: params)
    {
        std::optional<int> paramId = param.id();
        bool paramResult = paramId.has_value() ?
                               updateEnvParam(envId, param) :
                               insertEnvParam(envId, param);

        result = result && paramResult;

    }

    return result;
}

bool Db::insertCollection(Collection &collection)
{
    QSqlQuery insertCollectionQuery(m_db);
    insertCollectionQuery.prepare("INSERT INTO collections (name, parent_id) VALUES(:name, :parent_id);");
    insertCollectionQuery.bindValue(":name", collection.name());

    if (collection.parent().has_value())
    {
        insertCollectionQuery.bindValue(":parent_id", collection.parent().value());
    }
    else
    {
        // null value for parent
        insertCollectionQuery.bindValue(":parent_id", QVariant(QMetaType::fromType<int>()));
    }

    bool result = insertCollectionQuery.exec();

    if (!result)
    {
        qDebug() << insertCollectionQuery.lastError().text();
    }
    else
    {
        int collectionId = insertCollectionQuery.lastInsertId().toInt();
        collection.setId(collectionId);
    }

    return result;
}

bool Db::updateCollection(Collection &collection)
{
    QSqlQuery updateCollectionQuery(m_db);
    updateCollectionQuery.prepare("UPDATE collections SET name = :name WHERE id = :id;");
    updateCollectionQuery.bindValue(":name", collection.name());
    updateCollectionQuery.bindValue(":id", collection.id().value());

    return updateCollectionQuery.exec();
}

bool Db::deleteCollection(int collectionId)
{
    QSqlQuery deleteCollection(m_db);
    deleteCollection.prepare("DELETE from collections WHERE id = :collection_id;");
    deleteCollection.bindValue(":collection_id", collectionId);

    return deleteCollection.exec();
}

std::optional<Query> Db::getQuery(int queryId)
{
    QSqlQuery getQuery(m_db);
    getQuery.prepare("SELECT id, collection_id, name, method, url, auth_type, body_type "
                     "FROM queries "
                     "WHERE id = :id;");
    getQuery.bindValue(":id", queryId);

    if (!getQuery.exec() || !getQuery.next())
    {
        return std::nullopt;
    }

    Query query;
    query.setId(getQuery.value(0).toInt());
    query.setCollectionId(getQuery.value(1).toInt());
    QString name = getQuery.value(2).toString();
    QString method = getQuery.value(3).toString();
    QString url = getQuery.value(4).toString();
    query.setName(name);
    query.setMethod(method);
    query.setUrl(url);
    query.setAuthType(static_cast<Query::AuthType>(getQuery.value(5).toInt()));
    query.setBodyType(static_cast<Query::BodyType>(getQuery.value(6).toInt()));

    query.setParameters(getQueryParams(queryId));
    query.setHeaders(getQueryHeaders(queryId));

    if (query.authType() == Query::AuthType::Basic)
    {
        std::optional<BasicQueryAuth> basicAuth = getQueryBasicAuth(queryId);

        if (basicAuth.has_value())
        {
            query.setBasicAuth(basicAuth.value());
        }
    }

    if (query.authType() == Query::AuthType::BearerToken)
    {
        std::optional<BearerQueryAuth> bearerAuth = getQueryBearerAuth(queryId);

        if (bearerAuth.has_value())
        {
            query.setBearerAuth(bearerAuth.value());
        }
    }

    query.setMultipartFormBody(getQueryMultiPartBody(queryId));
    query.setEncodedFormBody(getQueryEncodedFormBody(queryId));

    std::optional<QueryBinaryBody> binaryBody = getQueryBinaryBody(queryId);

    if (binaryBody.has_value())
    {
        query.setBinaryBody(binaryBody.value());
    }

    std::optional<QueryRawBody> rawBody = getQueryRawBody(queryId);

    if (rawBody.has_value())
    {
        query.setRawBody(rawBody.value());
    }

    return query;
}

QList<ParamValue> Db::getQueryParams(int queryId)
{
    QList<ParamValue> result;

    QSqlQuery getParams(m_db);
    getParams.prepare("SELECT id, name, value, description FROM query_params WHERE query_id = :query_id;");
    getParams.bindValue(":query_id", queryId);

    getParams.exec();
    while (getParams.next())
    {
        int id = getParams.value(0).toInt();
        QString name = getParams.value(1).toString();
        QString value = getParams.value(2).toString();
        QString description = getParams.value(3).toString();

        ParamValue param(id, name, value, description);
        result.append(param);
    }

    return result;

}

QList<ParamValue> Db::getQueryHeaders(int queryId)
{
    QList<ParamValue> result;
    QSqlQuery getHeaders(m_db);
    getHeaders.prepare("SELECT id, name, value, description FROM query_headers WHERE query_id = :query_id;");
    getHeaders.bindValue(":query_id", queryId);

    getHeaders.exec();
    while (getHeaders.next())
    {
        int id = getHeaders.value(0).toInt();
        QString name = getHeaders.value(1).toString();
        QString value = getHeaders.value(2).toString();
        QString description = getHeaders.value(3).toString();

        ParamValue param(id, name, value, description);
        result.append(param);
    }

    return result;
}

QList<ParamValue> Db::getQueryMultiPartBody(int queryId)
{
    QList<ParamValue> result;
    QSqlQuery getMultiPartParams(m_db);

    getMultiPartParams.prepare("SELECT id, name, type, value, description FROM query_form_data_body WHERE query_id = :query_id;");
    getMultiPartParams.bindValue(":query_id", queryId);

    getMultiPartParams.exec();

    while (getMultiPartParams.next())
    {
        int id = getMultiPartParams.value(0).toInt();
        QString name = getMultiPartParams.value(1).toString();
        ParamValue::ParamValueType paramType = static_cast<ParamValue::ParamValueType>(getMultiPartParams.value(2).toInt());
        QString value = getMultiPartParams.value(2).toString();
        QString description = getMultiPartParams.value(3).toString();

        ParamValue paramValue(id, name, value, description);
        paramValue.setValueType(paramType);

        result.append(paramValue);
    }

    return result;
}

QList<ParamValue> Db::getQueryEncodedFormBody(int queryId)
{
    QList<ParamValue> result;
    QSqlQuery getEncodedFormParams(m_db);

    getEncodedFormParams.prepare("SELECT id, name, value, description FROM query_encoded_form_body WHERE query_id = :query_id;");
    getEncodedFormParams.bindValue(":query_id", queryId);

    getEncodedFormParams.exec();

    while (getEncodedFormParams.next())
    {
        int id = getEncodedFormParams.value(0).toInt();
        QString name = getEncodedFormParams.value(1).toString();
        QString value = getEncodedFormParams.value(2).toString();
        QString description = getEncodedFormParams.value(3).toString();

        ParamValue param(id, name, value, description);
        result.append(param);
    }

    return result;
}

std::optional<QueryRawBody> Db::getQueryRawBody(int queryId)
{
    QSqlQuery getRawBody(m_db);
    getRawBody.prepare("SELECT id, query_id, type, value FROM queries_raw_body WHERE query_id = :query_id;");
    getRawBody.bindValue(":query_id", queryId);

    getRawBody.exec();

    if (getRawBody.next())
    {
        int id = getRawBody.value(0).toInt();
        int queryId = getRawBody.value(1).toInt();
        QueryRawBody::RawBodyType type = static_cast<QueryRawBody::RawBodyType>(getRawBody.value(2).toInt());
        QString value = getRawBody.value(3).toString();
        QueryRawBody rawBody(id, queryId, type, value);

        return rawBody;
    }

    return std::nullopt;
}

std::optional<QueryBinaryBody> Db::getQueryBinaryBody(int queryId)
{
    QSqlQuery getBinaryBody(m_db);
    getBinaryBody.prepare("SELECT id, query_id, file_path from queries_binary_body WHERE query_id = :query_id;");
    getBinaryBody.bindValue(":query_id", queryId);

    getBinaryBody.exec();

    if (getBinaryBody.next())
    {
        int id = getBinaryBody.value(0).toInt();
        int queryId = getBinaryBody.value(1).toInt();
        QString filePath = getBinaryBody.value(2).toString();

        QueryBinaryBody binaryBody(id, queryId, filePath);
        return binaryBody;
    }

    return std::nullopt;
}

std::optional<BearerQueryAuth> Db::getQueryBearerAuth(int queryId)
{
    QSqlQuery getBearerAuth(m_db);
    getBearerAuth.prepare("SELECT id, query_id, bearer_token FROM queries_auth_bearer WHERE query_id = :query_id;");
    getBearerAuth.bindValue(":query_id", queryId);

    getBearerAuth.exec();

    if (getBearerAuth.next())
    {
        int id = getBearerAuth.value(0).toInt();
        int dbQueryId = getBearerAuth.value(1).toInt();
        QString bearerToken = getBearerAuth.value(2).toString();

        BearerQueryAuth bearerAuth(id, dbQueryId, bearerToken);
        return bearerAuth;
    }

    return std::nullopt;
}

std::optional<BasicQueryAuth> Db::getQueryBasicAuth(int queryId)
{
    QSqlQuery getBasicAuth(m_db);
    getBasicAuth.prepare("SELECT id, query_id, username, password FROM queries_auth_basic WHERE query_id = :query_id;");
    getBasicAuth.bindValue(":query_id", queryId);

    getBasicAuth.exec();

    if (getBasicAuth.next())
    {
        int id = getBasicAuth.value(0).toInt();
        int dbQueryId = getBasicAuth.value(1).toInt();
        QString username = getBasicAuth.value(2).toString();
        QString password = getBasicAuth.value(3).toString();

        BasicQueryAuth basicAuth(id, dbQueryId, username, password);

        return basicAuth;
    }

    return std::nullopt;
}

bool Db::saveQuery(Query &query)
{
    bool result = true;

    if (query.id().has_value())
    {
        result = updateQuery(query);
    }
    else
    {
        result = insertQuery(query);
    }

    result = result && deleteQueryDeletedParams(query);

    return result;
}

bool Db::deleteQuery(int queryId)
{
    QSqlQuery deleteQuery(m_db);

    deleteQuery.prepare("DELETE from queries WHERE id = :query_id");
    deleteQuery.bindValue(":query_id", queryId);

    return deleteQuery.exec();
}


QList<ParamValue> Db::getEnvParams(int envId)
{
    QSqlQuery getEnvParams(m_db);
    getEnvParams.prepare("SELECT id, name, value, description FROM envs_params WHERE env_id = :env_id;");
    getEnvParams.bindValue(":env_id", envId);
    QList<ParamValue> result;

    if (getEnvParams.exec())
    {
        while(getEnvParams.next())
        {
            int id = getEnvParams.value(0).toInt();
            QString name = getEnvParams.value(1).toString();
            QString value = getEnvParams.value(2).toString();
            QString description = getEnvParams.value(3).toString();
            ParamValue param(id, name, value, description);

            result.append(param);
        }
    }

    return result;
}

bool Db::insertEnvParam(int envId, ParamValue &paramValue)
{
    QSqlQuery insertEnvParam(m_db);
    insertEnvParam.prepare("INSERT INTO envs_params(env_id, name, value, description) "
                           "VALUES (:env_id, :name, :value, :description);");

    insertEnvParam.bindValue(":env_id", envId);
    insertEnvParam.bindValue(":name", paramValue.getValue("name"));
    insertEnvParam.bindValue(":value", paramValue.getValue("value"));
    insertEnvParam.bindValue(":description", paramValue.getValue("description"));

    bool result = insertEnvParam.exec();

    paramValue.setId(insertEnvParam.lastInsertId().toInt());

    return result;
}

bool Db::updateEnvParam(int envId, ParamValue &paramValue)
{
    QSqlQuery updateEnvParam(m_db);
    updateEnvParam.prepare("UPDATE envs_params SET "
                           "name = :name,"
                           "value = :value,"
                           "description = :description "
                           "WHERE id = :param_id;");

    updateEnvParam.bindValue(":name", paramValue.getValue("name"));
    updateEnvParam.bindValue(":value", paramValue.getValue("value"));
    updateEnvParam.bindValue(":description", paramValue.getValue("description"));
    updateEnvParam.bindValue(":param_id", paramValue.id().value());

    return updateEnvParam.exec();
}

bool Db::deleteEnvParams(QList<int> &idList)
{
    bool result = true;

    for (int &id : idList)
    {
        QSqlQuery deleteEnvParams(m_db);
        deleteEnvParams.prepare("DELETE FROM envs_params WHERE id = :id;");
        deleteEnvParams.bindValue(":id", id);

        result = result && deleteEnvParams.exec();
    }

    return result;
}

bool Db::insertQuery(Query &query)
{
    if (!query.collectionId().has_value())
    {
        return false;
    }

    QSqlQuery insert(m_db);
    insert.prepare("INSERT INTO queries(collection_id, name, method, url, auth_type) "
                   "VALUES(:collection_id, :name, :method, :url, :auth_type);");

    insert.bindValue(":collection_id", query.collectionId().value());
    insert.bindValue(":name", query.name());
    insert.bindValue(":method", query.method());
    insert.bindValue(":url", query.url());
    insert.bindValue(":auth_type", query.authType());

    bool insertResult = insert.exec();
    int id = insert.lastInsertId().toInt();

    if (!insertResult)
    {
        qDebug() << insert.lastError();
    }

    query.setId(id);

    // TODO add queries for all the other sections of the query
    insertResult = insertResult && saveQueryParams(query);
    insertResult = insertResult && saveQueryHeaders(query);
    insertResult = insertResult && saveQueryAuth(query);
    insertResult = insertResult && saveQueryBody(query);

    return insertResult;
}

bool Db::updateQuery(Query &query)
{
    if (!query.collectionId().has_value() || !query.id().has_value())
    {
        return false;
    }

    QSqlQuery update(m_db);
    update.prepare("UPDATE queries SET "
                   "collection_id = :collection_id,"
                   "name = :name,"
                   "method = :method,"
                   "url = :url "
                   "WHERE id = :query_id;");

    update.bindValue(":collection_id", query.collectionId().value());
    update.bindValue(":name", query.name());
    update.bindValue(":method", query.method());
    update.bindValue(":url", query.url());
    update.bindValue(":query_id", query.id().value());

    //TODO add queries for all other sections of the query

    bool updateResult = update.exec();

    updateResult = updateResult && saveQueryParams(query);
    updateResult = updateResult && saveQueryHeaders(query);
    updateResult = updateResult && saveQueryAuth(query);
    updateResult = updateResult && saveQueryBody(query);

    return updateResult;
}

bool Db::saveBasicQueryAuth(BasicQueryAuth &basicQueryAuth)
{
    if (basicQueryAuth.id().has_value())
    {
        return updateBasicQueryAuth(basicQueryAuth);
    }
    {
        return insertBasicQueryAuth(basicQueryAuth);
    }
}

bool Db::insertBasicQueryAuth(BasicQueryAuth &basicQueryAuth)
{
    QSqlQuery insertQueryAuth(m_db);
    bool insertResult = false;
    insertQueryAuth.prepare("INSERT INTO queries_auth_bascic(query_id, username, password) "
                            "VALUES (:query_id, :username, :password);");
    insertQueryAuth.bindValue(":query_id", basicQueryAuth.queryId().value());
    insertQueryAuth.bindValue(":username", basicQueryAuth.username());
    insertQueryAuth.bindValue(":password", basicQueryAuth.password());

    insertResult = insertQueryAuth.exec();

    int authId = insertQueryAuth.lastInsertId().toInt();

    basicQueryAuth.setId(authId);

    return insertResult;
}

bool Db::updateBasicQueryAuth(BasicQueryAuth &basicqQueryAuth)
{
    QSqlQuery updateAuth(m_db);
    updateAuth.prepare("UPDATE queries_auth SET "
                       "username = :username,"
                       "password = :password,"
                       "WHERE id = :id;");

    updateAuth.bindValue(":username", basicqQueryAuth.username());
    updateAuth.bindValue(":password", basicqQueryAuth.password());
    updateAuth.bindValue(":id", basicqQueryAuth.id().value());

    return updateAuth.exec();
}

bool Db::saveBearerQueryAuth(BearerQueryAuth &bearerQueryAuth)
{
    if (bearerQueryAuth.id().has_value())
    {
        return updateBearerQueryAuth(bearerQueryAuth);
    }
    else
    {
        return insertBearerQueryAuth(bearerQueryAuth);
    }
}

bool Db::insertBearerQueryAuth(BearerQueryAuth &bearerQueryAuth)
{
    QSqlQuery insertBearerAuth(m_db);

    insertBearerAuth.prepare("INSERT INTO queries_auth_bearer (query_id, bearer_token)"
                             "VALUES(:query_id, :bearer_token);");

    insertBearerAuth.bindValue(":query_id", bearerQueryAuth.id().value());
    insertBearerAuth.bindValue(":bearer_token", bearerQueryAuth.bearerToken());

    bool execResult = insertBearerAuth.exec();

    int id = insertBearerAuth.lastInsertId().toInt();
    bearerQueryAuth.setId(id);

    return execResult;
}

bool Db::updateBearerQueryAuth(BearerQueryAuth &bearerQueryAuth)
{
    QSqlQuery updateBearerAuth(m_db);

    updateBearerAuth.prepare("UPDATE queries_auth_bearer SET "
                             "bearer_token = :bearer_token"
                             "WHERE id = :id");

    updateBearerAuth.bindValue(":bearer_token", bearerQueryAuth.bearerToken());
    updateBearerAuth.bindValue(":id", bearerQueryAuth.id().value());

    return updateBearerAuth.exec();
}

bool Db::saveQueryAuth(Query &query)
{
    if (query.authType() == Query::AuthType::Basic)
    {
        return saveBasicQueryAuth(query.basicAuth());
    }

    if (query.authType() == Query::AuthType::BearerToken)
    {
        return saveBearerQueryAuth(query.bearerAuth());
    }

    return true;
}

bool Db::saveQueryHeaders(Query &query)
{
    bool result = true;

    QList<ParamValue> &headers = query.headers();
    for (int i = 0; i < headers.size(); i++)
    {
        ParamValue &header = headers[i];
        if (header.id().has_value())
        {
            result = result && updateQueryHeader(header);
        }
        else
        {
            result = result && insertQueryHeader(query.id().value(), header);
        }
    }

    return result;
}

bool Db::insertQueryHeader(int queryId, ParamValue &header)
{
    QSqlQuery insertHeader(m_db);
    insertHeader.prepare("INSERT INTO queries_headers(query_id, name, value, description) "
                         "VALUES (:query_id, :name, :value, :description);");
    insertHeader.bindValue(":query_id", queryId);
    insertHeader.bindValue(":name", header.getValue("name"));
    insertHeader.bindValue(":value", header.getValue("value"));
    insertHeader.bindValue(":description", header.getValue("description"));

    bool insertResult = insertHeader.exec();
    int headerId = insertHeader.lastInsertId().toInt();
    header.setId(headerId);

    return insertResult;
}

bool Db::updateQueryHeader(ParamValue &header)
{
    if (!header.id().has_value())
    {
        return false;
    }

    QSqlQuery updateHeader(m_db);
    updateHeader.prepare("UPDATE queries_headers SET "
                         "name = :name,"
                         "value= :value,"
                         "description = :description "
                         "WHERE id = :id");

    updateHeader.bindValue(":name", header.getValue("name"));
    updateHeader.bindValue(":value", header.getValue("value"));
    updateHeader.bindValue(":description", header.getValue("description"));
    updateHeader.bindValue(":id", header.id().value());

    return updateHeader.exec();
}

bool Db::deleteQueryHeaders(QList<int> &idList)
{
    bool result = true;

    for (int &id : idList)
    {
        QSqlQuery deleteHeader(m_db);
        deleteHeader.prepare("DELETE FROM queries_headers WHERE id = :id;");
        deleteHeader.bindValue(":id", id);
        result = result && deleteHeader.exec();
    }

    return result;
}

bool Db::saveQueryParams(Query &query)
{
    if (!query.id().has_value())
    {
        return false;
    }

    bool result = true;
    QList<ParamValue>& params = query.parameters();
    for (ParamValue& param : params)
    {
        if (param.id().has_value())
        {
            result = result && updateQueryParam(param);
        }
        else
        {
            result = result && insertQueryParam(query.id().value(), param);
        }
    }

    return result;
}

bool Db::insertQueryParam(int queryId, ParamValue &paramValue)
{
    QSqlQuery insertQueryParam(m_db);
    insertQueryParam.prepare("INSERT INTO queries_params(query_id, name, value, description) "
                             "VALUES(:query_id, :name, :value, :description);");

    insertQueryParam.bindValue(":query_id", queryId);
    insertQueryParam.bindValue(":name", paramValue.getValue("name"));
    insertQueryParam.bindValue(":value", paramValue.getValue("value"));
    insertQueryParam.bindValue(":description", paramValue.getValue("description"));

    bool execResult = insertQueryParam.exec();
    int id = insertQueryParam.lastInsertId().toInt();

    paramValue.setId(id);
    return execResult;
}

bool Db::updateQueryParam(ParamValue &paramValue)
{
    QSqlQuery updateQueryParam(m_db);
    updateQueryParam.prepare("UPDATE queries_params SET "
                             "name = :name,"
                             "value = :value,"
                             "description = :description "
                             "WHERE id = :param_id");
    updateQueryParam.bindValue(":name", paramValue.getValue("name"));
    updateQueryParam.bindValue(":value", paramValue.getValue("value"));
    updateQueryParam.bindValue(":description", paramValue.getValue("description"));

    bool execResult = updateQueryParam.exec();

    return execResult;
}

bool Db::deleteQueryParams(QList<int> &idList)
{
    bool result = true;

    for (int &id : idList)
    {
        QSqlQuery deleteParam(m_db);
        deleteParam.prepare("DELETE FROM queries_params WHERE id = :id;");
        deleteParam.bindValue(":id", id);
        result = result && deleteParam.exec();
    }

    return result;
}

bool Db::saveQueryBody(Query &query)
{
    bool result = false;

    switch (query.bodyType())
    {
    case Query::BodyType::MultipartForm:
        result = saveQueryBodyFormData(query);
        break;
    case Query::BodyType::EncodedForm:
        result = saveQueryBodyEncodedFormData(query);
        break;
    case Query::BodyType::Raw:
        result = saveQueryRawBody(query);
        break;
    case Query::BodyType::Binary:
        result = saveQueryBinaryBody(query);
        break;
    case Query::BodyType::Empty:
        result = true;
        break;
    default:
        result = false;
        break;
    }

    return result;
}

bool Db::saveQueryBodyFormData(Query &query)
{
    bool result = true;

    for (ParamValue &paramValue : query.multipartFormBody())
    {
        if (paramValue.id().has_value())
        {
            result = result && updateQueryBodyFormDataParam(paramValue);
        }
        else
        {
            result = result && insertQueryBodyFormDataParam(query.id().value(), paramValue);
        }
    }

    return result;
}

bool Db::insertQueryBodyFormDataParam(int queryId, ParamValue &paramValue)
{
    QSqlQuery insertBodyFormDataParam(m_db);
    insertBodyFormDataParam.prepare("INSERT INTO queries_form_data_body(query_id, name, type, value, description)"
                                    "VALUES(:query_id, :name, :type, :value, :description);");

    insertBodyFormDataParam.bindValue(":query_id", queryId);
    insertBodyFormDataParam.bindValue(":name", paramValue.getValue("name"));
    insertBodyFormDataParam.bindValue(":type", paramValue.getValueType());
    insertBodyFormDataParam.bindValue(":value", paramValue.getValue("value"));
    insertBodyFormDataParam.bindValue(":description", paramValue.getValue("description"));

    bool execResult = insertBodyFormDataParam.exec();

    int id = insertBodyFormDataParam.lastInsertId().toInt();
    paramValue.setId(id);

    return execResult;
}

bool Db::updateQueryBodyFormDataParam(ParamValue &paramValue)
{
    QSqlQuery updateBodyFormDataParam(m_db);
    updateBodyFormDataParam.prepare("UPDATE queries_form_data_body SET"
                                    "name = :name,"
                                    "type = :type,"
                                    "value = :value,"
                                    "description = :description "
                                    "WHERE id = :id");

    updateBodyFormDataParam.bindValue(":name", paramValue.getValue("name"));
    updateBodyFormDataParam.bindValue(":type", paramValue.getValueType());
    updateBodyFormDataParam.bindValue(":value", paramValue.getValue("value"));
    updateBodyFormDataParam.bindValue(":description", paramValue.getValue("description"));
    updateBodyFormDataParam.bindValue(":id", paramValue.id().value());

    return updateBodyFormDataParam.exec();
}

bool Db::deleteQueryBodyMultipartParams(QList<int> &idParams)
{
    bool result = true;

    for (int &id: idParams)
    {
        QSqlQuery deleteParam(m_db);
        deleteParam.prepare("DELETE FROM queries_form_data_body WHERE id = :id;");
        deleteParam.bindValue(":id", id);
        result = result && deleteParam.exec();
    }

    return result;
}

bool Db::saveQueryBodyEncodedFormData(Query &query)
{
    bool result = true;

    for (ParamValue &paramValue : query.encodedFormBody())
    {
        if (paramValue.id().has_value())
        {
            result = result && updateQueryEncodedFormData(paramValue);
        }
        else
        {
            result = result && insertQueryEncodedFormData(query.id().value(), paramValue);
        }
    }

    return result;
}

bool Db::insertQueryEncodedFormData(int queryId, ParamValue &paramValue)
{
    QSqlQuery insertEncodedFormData(m_db);

    insertEncodedFormData.prepare("INSERT INTO queries_encoded_form_body(query_id, name, value, description)"
                                  "VALUES(:query_id, :name, :value, :description);");

    insertEncodedFormData.bindValue(":query_id", queryId);
    insertEncodedFormData.bindValue(":name", paramValue.getValue("name"));
    insertEncodedFormData.bindValue(":value", paramValue.getValue("value"));
    insertEncodedFormData.bindValue(":description", paramValue.getValue("description"));

    bool execResult = insertEncodedFormData.exec();

    int id = insertEncodedFormData.lastInsertId().toInt();
    paramValue.setId(id);

    return execResult;
}

bool Db::updateQueryEncodedFormData(ParamValue &paramValue)
{
    QSqlQuery updateEncodedFormData(m_db);

    updateEncodedFormData.prepare("UPDATE queries_encoded_form_body SET "
                                  "name = :name,"
                                  "value = :value,"
                                  "description = :description "
                                  "WHERE id = :id;");

    updateEncodedFormData.bindValue(":name", paramValue.getValue("name"));
    updateEncodedFormData.bindValue(":value", paramValue.getValue("value"));
    updateEncodedFormData.bindValue(":description", paramValue.getValue("description"));
    updateEncodedFormData.bindValue(":id", paramValue.id().value());

    return updateEncodedFormData.exec();
}

bool Db::deleteQueryEncodedForm(QList<int> &idParams)
{
    bool result = true;
    for (int &id : idParams)
    {
        QSqlQuery deleteParam(m_db);
        deleteParam.prepare("DELETE FROM queries_encoded_form_body WHERE id = :id;");
        deleteParam.bindValue(":id", id);

        result = result && deleteParam.exec();
    }

    return result;
}

bool Db::saveQueryRawBody(Query &query)
{
    if (query.rawBody().value().id().has_value())
    {
        return updateQueryRawBody(query.rawBody().value());
    }
    else
    {
        return insertQueryRawBody(query.rawBody().value());
    }
}

bool Db::insertQueryRawBody(QueryRawBody &rawBody)
{
    QSqlQuery insertRawBody(m_db);
    insertRawBody.prepare("INSERT INTO queries_raw_body(query_id, type, value) "
                          "VALUES(:query_id, :type, :value);");
    insertRawBody.bindValue(":query_id", rawBody.queryId().value());
    insertRawBody.bindValue(":type", rawBody.rawBodyType());
    insertRawBody.bindValue(":value", rawBody.value());

    bool execResult = insertRawBody.exec();

    int id = insertRawBody.lastInsertId().toInt();

    rawBody.setId(id);

    return execResult;
}

bool Db::updateQueryRawBody(QueryRawBody &rawBody)
{
    QSqlQuery updateRawBody(m_db);

    updateRawBody.prepare("UPDATE queries_raw_body SET "
                          "type = :type,"
                          "value = :value "
                          "WHERE id = :id;");
    updateRawBody.bindValue(":type", rawBody.rawBodyType());
    updateRawBody.bindValue(":value", rawBody.value());
    updateRawBody.bindValue(":id", rawBody.id().value());

    return updateRawBody.exec();
}

bool Db::saveQueryBinaryBody(Query &query)
{
    if (query.binaryBody().value().id().has_value())
    {
        return updateQueryBinaryBody(query.binaryBody().value());
    }
    else
    {
        return insertQueryBinaryBody(query.binaryBody().value());
    }
}

bool Db::insertQueryBinaryBody(QueryBinaryBody &binaryBody)
{
    QSqlQuery insertBinaryBody(m_db);

    insertBinaryBody.prepare("INSERT into queries_binary_body(query_id, file_path)"
                             "VALUES(:query_id, :filepath);");
    insertBinaryBody.bindValue(":query_id", binaryBody.queryId().value());
    insertBinaryBody.bindValue(":file_path", binaryBody.filepath());

    bool execResult = insertBinaryBody.exec();

    int id = insertBinaryBody.lastInsertId().toInt();

    binaryBody.setId(id);

    return execResult;
}

bool Db::updateQueryBinaryBody(QueryBinaryBody &binaryBody)
{
    QSqlQuery updateBinaryBody(m_db);

    updateBinaryBody.prepare("UPDATE queries_binary_body SET "
                             "file_path = :file_path "
                             "WHERE id = :id;");
    updateBinaryBody.bindValue(":file_path", binaryBody.filepath());
    updateBinaryBody.bindValue(":id", binaryBody.id().value());

    return updateBinaryBody.exec();
}

bool Db::deleteQueryDeletedParams(Query &query)
{
    bool result = true;

    if (!query.deletedParams().empty())
    {
        result = result && deleteQueryParams(query.deletedParams());
    }

    if (!query.deletedHeaders().empty())
    {
        result = result && deleteQueryHeaders(query.deletedHeaders());
    }

    if (!query.deletedMultipartParams().empty())
    {
        result = result && deleteQueryBodyMultipartParams(query.deletedMultipartParams());
    }

    if (!query.deletedEncodedFormParams().empty())
    {
        result = result && deleteQueryEncodedForm(query.deletedEncodedFormParams());
    }

    return result;
}
