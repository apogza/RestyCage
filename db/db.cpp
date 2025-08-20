#include "db.h"
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
            "name TEXT);"
            );

        if (createEnvsTable.exec())
        {
            qDebug() << "Created envs table";
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


