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

    if (!tables.contains(queriesAuthTable))
    {
        QSqlQuery createQueriesAuthTable(m_db);
        bool execResult = createQueriesAuthTable.exec(
            "CREATE TABLE queries_auth("
            "id INTEGER PRIMARY KEY,"
            "query_id INTEGER,"
            "auth_type INTEGER,"
            "username TEXT,"
            "password TEXT,"
            "bearer_token TEXT,"
            "FOREIGN KEY (query_id) REFERENCES queries(id) ON DELETE CASCADE ON UPDATE NO ACTION);");

        if (execResult)
        {
            qDebug() << "Create queries auth table";
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

    result = result && deleteEnvParameters(env.deletedParams());

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
        for (ParamValue param: envParams)
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

bool Db::deleteEnvParameters(const QList<int> &deletedParams)
{
    bool result = true;
    for (int i = 0; i < deletedParams.size(); i++)
    {
        QSqlQuery deletedParam(m_db);
        deletedParam.prepare("DELETE FROM envs_params WHERE id= :param_id;");
        deletedParam.bindValue(":param_id", deletedParams[i]);

        bool deleteResult = deletedParam.exec();

        result = result && deleteResult;
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

bool Db::saveQuery(Query &query)
{
    if (query.id().has_value())
    {
        return updateQuery(query);
    }
    else
    {
        return insertQuery(query);
    }
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

bool Db::insertQuery(Query &query)
{
    if (!query.collectionId().has_value())
    {
        return false;
    }

    QSqlQuery insert(m_db);
    insert.prepare("INSERT INTO queries(collection_id, name, method, url) "
                   "VALUES(:collection_id, :name, :method, :url);");

    insert.bindValue(":collection_id", query.collectionId().value());
    insert.bindValue(":name", query.name());
    insert.bindValue(":method", query.method());
    insert.bindValue(":url", query.url());

    bool insertResult = insert.exec();
    int id = insert.lastInsertId().toInt();

    query.setId(id);

    // TODO add queries for all the other sections of the query
    insertResult = insertResult && saveQueryParams(query);
    insertResult = insertResult && saveQueryHeaders(query);
    insertResult = insertResult && saveQueryAuth(query);

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

    return updateResult;
}

bool Db::saveQueryAuth(Query &query)
{
    if (query.auth().id().has_value())
    {
        return updateQueryAuth(query);
    }
    {
        return insertQuery(query);
    }
}

bool Db::insertQueryAuth(Query &query)
{
    if (!query.id().has_value())
    {
        return false;
    }

    QueryAuth &auth = query.auth();

    if (auth.authType() == QueryAuth::AuthType::None)
    {
        return true;
    }

    QSqlQuery insertQueryAuth(m_db);
    insertQueryAuth.prepare("INSERT INTO queries_auth(query_id, username, password, bearer_token) "
                            "VALUES (:query_id, :username, :password, :bearer_token);");

    insertQueryAuth.bindValue(":query_id", query.id().value());

    if (auth.authType() == QueryAuth::AuthType::Basic)
    {
        insertQueryAuth.bindValue(":username", auth.username());
        insertQueryAuth.bindValue(":password", auth.password());
        insertQueryAuth.bindValue(":bearer_token", QVariant(QMetaType::fromType<QString>()));
    }
    else
    {
        insertQueryAuth.bindValue(":username", QVariant(QMetaType::fromType<QString>()));
        insertQueryAuth.bindValue(":password", QVariant(QMetaType::fromType<QString>()));
        insertQueryAuth.bindValue(":bearer_token", auth.bearerToken());
    }

    bool insertResult = insertQueryAuth.exec();
    int authId = insertQueryAuth.lastInsertId().toInt();
    auth.setId(authId);

    return insertResult;
}

bool Db::updateQueryAuth(Query &query)
{
    if (!query.id().has_value())
    {
        return false;
    }

    QueryAuth &auth = query.auth();

    if (auth.authType() == QueryAuth::AuthType::None)
    {
        QSqlQuery deleteAuth(m_db);
        deleteAuth.prepare("DELETE FROM queries_auth WHERE query_id = :query_id");
        deleteAuth.bindValue(":query_id", query.id().value());

        return deleteAuth.exec();
    }

    QSqlQuery updateAuth(m_db);
    updateAuth.prepare("UPDATE queries_auth SET "
                       "username = :username,"
                       "password = :password,"
                       "bearer_token = :bearer_token "
                       "WHERE id = :id;");


    if (auth.authType() == QueryAuth::AuthType::Basic)
    {
        updateAuth.bindValue(":username", auth.username());
        updateAuth.bindValue(":password", auth.password());
        updateAuth.bindValue(":bearer_token", QVariant(QMetaType::fromType<QString>()));
    }
    else
    {
        updateAuth.bindValue(":username", QVariant(QMetaType::fromType<QString>()));
        updateAuth.bindValue(":password", QVariant(QMetaType::fromType<QString>()));
        updateAuth.bindValue(":bearer_token", auth.bearerToken());
    }

    return updateAuth.exec();
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
