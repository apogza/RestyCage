#ifndef DB_H
#define DB_H

#include "environment.h"
#include <QtSql/QSqlDatabase>

class Db
{
private:
    Db();
    void init();

    void initEnvs();
    void initCollections();
    void initQueries();

    bool saveEnv(Environment &environment);

    QSqlDatabase m_db;
public:
    Db &operator=(const Db &) = delete;
    static Db &instance();
    void close();
    QList<Environment> getEnvs();
    std::optional<Environment> getEnv(int envId);

    bool insertEnv(Environment &environment);
    bool updateEnv(Environment &environment);

    bool insertEnvParam(int envId, ParamValue &paramValue);
    bool updateEnvParam(int envId, ParamValue &paramValue);
};

#endif // DB_H
