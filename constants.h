#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>
#include <QByteArray>
#include <QGlobalStatic>
#include <QStandardPaths>
#include <QDir>

enum BodyType { Empty, MultipartForm, EncodedForm, Raw, Binary };
enum AuthType { None, Basic, BearerToken };

const QString settingsOrgKey = "NoOrg";
const QString settingsAppKey = "RestyCage";
const QString activeEnvironmentId = "activeEnvironmentId";
const QString authTypeNone = "None";
const QString authTypeBasicAuth = "Basic Auth";
const QString authTypeBearerToken = "Bearer Token";
const QString bodyTypeNone = "None";
const QString bodyTypeFormData = "Form Data";
const QString bodyTypeRaw = "Raw";
const QString bodyTypeEncodedForm = "Encoded Form";
const QString bodyTypeBinary = "Binary";

const QString serializationUuid = "uuid";
const QString serializationBasicAuthId = "basicAuthId";
const QString serializationBearerTokenId = "bearerTokenId";
const QString serializationBinaryBodyId = "binaryBodyId";
const QString serializationRawBodyId = "rawBodyId";
const QString serializationId = "id";
const QString serializationCollectionId = "collectionId";
const QString serializationName = "name";
const QString serializationUrl = "url";
const QString serializationMethod = "method";
const QString serializationUsername = "username";
const QString serializationPassword = "password";
const QString serializationBearerToken = "bearerToken";
const QString serializationAuthentication = "authentication";
const QString serializationBody = "body";
const QString serializationReplyBody = "replyBody";
const QString serializationReplyType = "replyType";
const QString serializationReplyHeaders = "replyHeaders";
const QString serializationBodyType = "bodyType";
const QString serializationHeaders = "headers";
const QString serializationParams = "params";

const QString paramId = "id";
const QString paramKey = "key";
const QString paramValue = "value";
const QString paramFilePathValue = "filePathValue";
const QString paramType = "type";
const QString paramTypeFile = "File";
const QString paramTypeText = "Text";
const QString paramDescription = "description";

const QString requestName = "name";
const QString requestValue = "value";
const QString requestDescription = "description";

const QString dbPath("./resty_cage.db");
const QString collectionsTable = "collections";
const QString queriesTable = "queries";
const QString queriesParamsTable = "queries_params";
const QString queriesHeadersTable = "queries_headers";
const QString queriesBasicAuthTable = "queries_auth_basic";
const QString queriesBearerAuthTable = "queries_auth_bearer";
const QString queriesFormDataBodyTable = "queries_form_data_body";
const QString queriesEncodedFormBodyTable = "queries_enoded_form_body";
const QString queriesRawBodyTable = "queries_raw_body";
const QString queriesBinaryBodyTable = "queries_binary_body";
const QString envsTable = "envs";
const QString envsParamsTable = "envs_params";

const QByteArray authorizationHeader = "Authorization";

static const char *nameHeader = "Name";
static const char *valueHeader = "Value";
static const char *descriptionHeader = "Description";
static const char *typeHeader = "Type";

#endif // CONSTANTS_H
