#ifndef MIGRATION_H
#define MIGRATION_H

#include<QString>
#include <QSqlDatabase>

class Migration
{
public:
    Migration(QString id, QSqlDatabase &db);

    virtual bool apply();

protected:
    QString m_id;
    QSqlDatabase m_db;
    virtual bool run() = 0;

};

#endif // MIGRATION_H
