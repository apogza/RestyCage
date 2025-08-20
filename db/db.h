#ifndef DB_H
#define DB_H

#include <QtSql/QSqlDatabase>

class Db
{
private:
    Db();
    void init();

    void initEnvs();
    void initCollections();
    void initQueries();

    QSqlDatabase m_db;
public:
    Db &operator=(const Db &) = delete;
    static Db &instance();

    void close();
};

#endif // DB_H
