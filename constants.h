#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>
#include <QByteArray>
#include <QGlobalStatic>
#include <QStandardPaths>

const QString appDataFolder = QString("%1/%2").arg(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation), "resty_cage");
const QString envDirPath = QString("%1/%2").arg(appDataFolder, "envs");
const QString collectionDirPath = QString("%1/%2").arg(appDataFolder, "collections");
const QString dbPath = QString("%1/%2").arg(appDataFolder, "resty_cage.db");
const QString collectionsTable = "collections";
const QString queriesTable = "queries";
const QString queriesParamsTable = "queries_params";
const QString queriesHeadersTable = "queries_headers";
const QString collectionsQueriesTable = "collections_queries";
const QString envsTable = "envs";
const QString envsParamsTable = "envs_params";
const QString querySerializationType = "type";
const QString querySerializationAuthorization = "authorization";
const QString querySerializationUsername = "username";
const QString querySerializationPassword = "password";
const QString querySerializationBasic = "basic";
const QString querySerializationName = "name";
const QString querySerializationMethod = "method";
const QString querySerializationUrl = "url";
const QString querySerializationParams = "parameters";
const QString querySerializationHeaders = "headers";
const QString querySerializationBearer = "bearer";
const QString querySerializationBody = "body";
const QString querySerializationValue = "value";
const QByteArray authorizationHeader = "Authorization";

static const char *keyHeader = "Key";
static const char *valueHeader = "Value";
static const char *descriptionHeader = "Description";
static const char *typeHeader = "Type";

#endif // CONSTANTS_H
