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
            "collection_id INTEGER,"
            "name TEXT,"
            "FOREIGN KEY (collection_id) REFERENCES collections(id) ON DELETE CASCADE ON UPDATE NO ACTION);"
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
    QVariant &envId = env.id();

    if (envId.isNull())
    {
        return insertEnv(env);
    }
    else
    {
        return updateEnv(env);
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

QList<Environment> Db::getEnvs()
{
    QList<Environment> result;

    QSqlQuery envsQuery("SELECT id, name, active from envs;");

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
    QSqlQuery envQuery;
    envQuery.prepare("SELECT id, name, active from envs WHERE id = :env_id");
    envQuery.bindValue(":env_id", envId);

    if (envQuery.exec() && envQuery.next())
    {
        int id = envQuery.value(0).toInt();
        QString name = envQuery.value(1).toString();
        bool active = envQuery.value(2).toBool();

        return std::make_optional<Environment>({id, name, active});
    }

    return std::nullopt;
}

bool Db::insertEnv(Environment &environment)
{
    QSqlQuery insertEnv;
    insertEnv.prepare("INSERT INTO envs(name, active)"
                      "VALUES(:name, :active)");
    insertEnv.bindValue(":name", environment.name());
    insertEnv.bindValue(":active", environment.active());

    bool result = insertEnv.exec();

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
    QSqlQuery updateEnv;

    updateEnv.prepare("UPDATE envs SET"
                      "name = :name,"
                      "active= :active,"
                      "WHERE id = :id;");

    updateEnv.bindValue(":name", environment.name());
    updateEnv.bindValue(":active", environment.active());
    updateEnv.bindValue(":id", environment.id());

    bool result = updateEnv.exec();

    QList<ParamValue> &params = environment.params();
    int envId = environment.id().toInt();

    for (int i = 0; i < params.size(); i++)
    {
        if (params[i].id().isNull())
        {
            result = result && insertEnvParam(envId, params[i]);
        }
        else
        {
            result = result && updateEnvParam(envId, params[i]);
        }
    }

    return result;
}

bool Db::insertEnvParam(int envId, ParamValue &paramValue)
{
    QSqlQuery insertEnvParam;
    insertEnvParam.prepare("INSERT envs_params(env_id, name, value, description) "
                           "VALUES(:env_id, :name, :value, :description);");

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
    QSqlQuery updateEnvParam;
    updateEnvParam.prepare("UPDATE envs_params SET"
                           "name = :name,"
                           "value = :value,"
                           "description = :description "
                           "WHERE env_id = :env_id;");

    updateEnvParam.bindValue(":name", paramValue.getValue("name"));
    updateEnvParam.bindValue(":value", paramValue.getValue("value"));
    updateEnvParam.bindValue(":description", paramValue.getValue("description"));

    return updateEnvParam.exec();
}
