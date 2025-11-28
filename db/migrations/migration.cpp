#include "migration.h"

#include <QSqlQuery>
#include <QDateTime>

Migration::Migration(QString id, QSqlDatabase &db)
{
    m_id = id;
    m_db = db;
}

bool Migration::apply()
{
    bool result = true;

    QSqlQuery checkMigration(m_db);
    checkMigration.prepare("SELECT id from migrations WHERE id = :id");
    checkMigration.bindValue(":id", m_id);

    if (checkMigration.next())
    {
        result = false;
    }
    else
    {
        result = run();

        if (result)
        {
            QSqlQuery insertMigration(m_db);
            insertMigration.prepare("INSERT INTO migrations(id, execution_date) VALUES(:id, :execution_date);");
            insertMigration.bindValue(":id", m_id);
            insertMigration.bindValue(":execution_date", QDateTime::currentDateTimeUtc());

            result = result && insertMigration.exec();
        }
    }

    return result;
}
