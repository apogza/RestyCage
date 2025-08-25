#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>
#include <QByteArray>
#include <QGlobalStatic>
#include <QStandardPaths>

const QString appDataFolder = QString("%1/%2").arg(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation), "resty_cage");
const QString dbPath = QString("%1/%2").arg(appDataFolder, "resty_cage.db");
const QString collectionsTable = "collections";
const QString queriesTable = "queries";
const QString queriesParamsTable = "queries_params";
const QString queriesHeadersTable = "queries_headers";
const QString queriesAuthTable = "queries_auth";
const QString queriesFormDataBodyTable = "queries_form_data_body";
const QString queriesEncodedFormBodyTable = "queries_enoded_form_body";
const QString queriesRawBodyTable = "queries_raw_body";
const QString queriesBinaryBodyTable = "queries_binary_body";
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
