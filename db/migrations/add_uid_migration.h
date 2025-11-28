#ifndef ADD_UID_MIGRATION_H
#define ADD_UID_MIGRATION_H

#include <QString>
#include "migration.h"

class AddUidMigration : public Migration
{
public:
    AddUidMigration(QString id, QSqlDatabase &db);

protected:
    virtual bool run() override;
};

#endif // ADD_UID_MIGRATION_H
