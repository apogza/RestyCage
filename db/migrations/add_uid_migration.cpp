#include "add_uid_migration.h"

#include <QSqlQuery>
#include <QUuid>
#include <QSqlError>

AddUidMigration::AddUidMigration(QString id, QSqlDatabase &db) : Migration(id, db) {}

bool AddUidMigration::run()
{
    QSqlQuery addQueryUid(m_db);
    bool addQueryUidResult = addQueryUid.exec("ALTER TABLE queries ADD COLUMN uid TEXT;");

    QSqlQuery addEnvUid(m_db);
    bool addEnvUidResult = addEnvUid.exec("ALTER TABLE envs add COLUMN uid TEXT;");

    QSqlQuery selectQueryIds("SELECT id from queries", m_db);

    bool updateQueryUidResult = true;
    QList<int> queryIds;
    while (selectQueryIds.next())
    {
        int id = selectQueryIds.value(0).toInt();
        queryIds << id;
    }

    for (int &id : queryIds)
    {
        QSqlQuery updateQueryUid(m_db);
        updateQueryUid.prepare("UPDATE queries SET uid = :uid WHERE id = :id;");

        updateQueryUid.bindValue(":uid", QUuid::createUuid());
        updateQueryUid.bindValue(":id", id);
        updateQueryUidResult = updateQueryUidResult && updateQueryUid.exec();
    }

    QSqlQuery selectEnvIds("SELECT id from envs", m_db);
    bool updateEnvUidResult = true;
    QList<int> envIds;
    while (selectEnvIds.next())
    {
        int id = selectEnvIds.value(0).toInt();
        envIds << id;
    }

    for (int &id : envIds)
    {
        QSqlQuery updateEnvUid(m_db);
        updateEnvUid.prepare("UPDATE envs SET uid = :uid WHERE id = :id;");

        updateEnvUid.bindValue(":uid", QUuid::createUuid());
        updateEnvUid.bindValue(":id", id);
        updateEnvUidResult = updateEnvUidResult && updateEnvUid.exec();
    }

    return addQueryUidResult && updateQueryUidResult && addEnvUidResult && updateEnvUidResult;
}
