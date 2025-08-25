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

    QStringList tables = m_db.tables();

    for (int i = 0; i < tables.size(); i++)
    {
        qDebug() << tables[i];
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
        createEnvsTable.prepare(
            "CREATE TABLE envs("
            "id INTEGER PRIMARY KEY,"
            "name TEXT NOT NULL,"
            "active INTEGER);"
            );

        if (createEnvsTable.exec())
        {
            qDebug() << "Created envs table";
        }
    }

    if (!tables.contains(envsParamsTable))
    {
        QSqlQuery createEnvsParamsTable(m_db);
        createEnvsParamsTable.prepare(
            "CREATE TABLE envs_params("
            "id INTEGER PRIMARY KEY,"
            "env_id INTEGER,"
            "name TEXT,"
            "value TEXT,"
            "description TEXT,"
            "FOREIGN KEY (env_id) REFERENCES envs on DELETE CASCADE ON UPDATE NO ACTION);"
            );

        if (createEnvsParamsTable.exec())
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
        createCollectionsTable.prepare(
            "CREATE TABLE collections("
            "id INTEGER PRIMARY KEY,"
            "parent_id INTEGER,"
            "name TEXT,"
            "FOREIGN KEY (parent_id) REFERENCES collections(id) ON DELETE CASCADE ON UPDATE NO ACTION);"
            );

        if (createCollectionsTable.exec())
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
        createQueriesTable.prepare(
            "CREATE TABLE queries("
            "id INTEGER PRIMARY KEY,"
            "collection_id INTEGER,"
            "name TEXT,"
            "method TEXT,"
            "url TEXT,"
            "auth_type INTEGER,"
            "auth TEXT,"
            "FOREIGN KEY (collection_id) REFERENCES collections(id) ON DELETE CASCADE ON UPDATE NO ACTION);"
            );

        if (createQueriesTable.exec())
        {
            qDebug() << "Created queries table";
        }
    }


    if (!tables.contains(queriesParamsTable))
    {
        QSqlQuery createQueriesParamsTable(m_db);
        createQueriesParamsTable.prepare(
            "CREATE TABLE queries_params("
            "id INTEGER PRIMARY KEY,"
            "query_id INTEGER,"
            "key TEXT,"
            "value TEXT,"
            "description TEXT,"
            "FOREIGN KEY (query_id) REFERENCES queries(id) ON DELETE CASCADE ON UPDATE NO ACTION);"
            );

        if (createQueriesParamsTable.exec())
        {
            qDebug() << "Create queries params table";
        }
    }

    if (!tables.contains(queriesHeadersTable))
    {
        QSqlQuery createQueriesHeadersTable(m_db);

        createQueriesHeadersTable.prepare(
            "CREATE TABLE queries_headers("
            "ID INTEGER PRIMARY KEY,"
            "query_id INTEGER,"
            "key TEXT,"
            "value TEXT,"
            "description TEXT,"
            "FOREIGN KEY (query_id) REFERENCES queries(id) ON DELETE CASCADE ON UPDATE NO ACTION);"
            );

        if (createQueriesHeadersTable.exec())
        {
            qDebug() << "Create queries headers table";
        }
    }

    if (!tables.contains(collectionsQueriesTable))
    {
        QSqlQuery createCollectionsQueriesTable(m_db);

        createCollectionsQueriesTable.prepare(
            "CREATE TABLE collections_queries("
            "collection_id INTEGER,"
            "query_id INTEGER,"
            "FOREIGN KEY (collection_id) REFERENCES collections(id) ON DELETE CASCADE ON UPDATE NO ACTION,"
            "FOREIGN KEY (query_id) REFERENCES queries(id) ON DELETE CASCADE ON UPDATE NO ACTION);");

        if (createCollectionsQueriesTable.exec())
        {
            qDebug() << "Create collections queries table";
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
        for (int i = 0; i < envParams.size(); i++)
        {
            env.addParam(envParams[i]);
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

    for (int i = 0 ; i < params.size(); i++)
    {
        result = result && insertEnvParam(envId, params[i]);
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

    for (int i = 0; i < params.size(); i++)
    {
        std::optional<int> paramId = params[i].id();
        bool paramResult = paramId.has_value() ?
                               updateEnvParam(envId, params[i]) :
                               insertEnvParam(envId, params[i]);

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
