#include "query.h"


Query::AuthType Query::authTypeFromString(const QString &authType)
{
    if (authType == "Bearer Token")
    {
        return AuthType::BearerToken;
    }
    else if (authType == "Basic Auth")
    {
        return AuthType::Basic;
    }
    else
    {
        return AuthType::None;
    }
}

QString Query::authTypeToString(const AuthType authType)
{
    QString result;

    switch (authType) {
    case AuthType::None:
        result = "None";
        break;
    case AuthType::Basic:
        result = "Basic Auth";
        break;
    case AuthType::BearerToken:
        result = "Bearer Token";
        break;
    default:
        break;
    }

    return result;
}

Query::Query()
{
}

Query::Query(std::optional<int> id, QString &name)
{
    m_id = id;
    m_name = name;
}

std::optional<int> Query::id()
{
    return m_id;
}

std::optional<int> Query::collectionId()
{
    return m_collectionId;
}

QString &Query::name()
{
    return m_name;
}

QString &Query::method()
{
    return m_method;
}

QString &Query::url()
{
    return m_url;
}

Query::AuthType Query::authType()
{
    return m_authType;
}

std::optional<BasicQueryAuth> &Query::basicAuth()
{
    return m_basicAuth;
}

std::optional<BearerQueryAuth> &Query::bearerAuth()
{
    return m_bearerAuth;
}

QList<ParamValue> &Query::parameters()
{
    return m_parameters;
}

QList<ParamValue> &Query::headers()
{
    return m_headers;
}

QList<ParamValue> &Query::encodedFormBody()
{
    return m_encodedFormParams;
}

QList<ParamValue> &Query::multipartFormBody()
{
    return m_multipartFormParams;
}

std::optional<QueryRawBody> &Query::rawBody()
{
    return m_queryRawBody;
}

std::optional<QueryBinaryBody> &Query::binaryBody()
{
    return m_queryBinaryBody;
}

QList<int> &Query::deletedParams()
{
    return m_deletedParams;
}

QList<int> &Query::deletedHeaders()
{
    return m_deletedHeaders;
}

QList<int> &Query::deletedMultipartParams()
{
    return m_deletedMultiPartFormParams;
}

QList<int> &Query::deletedEncodedFormParams()
{
    return m_deletedEncodedFormParams;
}

void Query::setId(int id)
{
    m_id = id;

    if (m_queryRawBody.has_value())
    {
        m_queryRawBody.value().setQueryId(id);
    }

    if (m_queryBinaryBody.has_value())
    {
        m_queryBinaryBody.value().setQueryId(id);
    }
}

void Query::setCollectionId(int collectionId)
{
    m_collectionId = collectionId;
}

void Query::setName(QString &name)
{
    m_name = name;
}

void Query::setMethod(QString &method)
{
    m_method = method;
}

void Query::setUrl(QString &url)
{
    m_url = url;
}

void Query::setAuthType(AuthType authType)
{
    m_authType = authType;
}

void Query::setBasicAuth(BasicQueryAuth& auth)
{
    m_basicAuth = auth;
}

void Query::setBearerAuth(BearerQueryAuth &auth)
{
    m_bearerAuth = auth;
}

void Query::setBodyType(BodyType bodyType)
{
    m_bodyType = bodyType;
}

void Query::setParameters(QList<ParamValue> parameters)
{
    m_parameters = parameters;
}

void Query::setHeaders(QList<ParamValue> headers)
{
    m_headers = headers;
}

void Query::setEncodedFormBody(QList<ParamValue> encodedFormParams)
{
    m_encodedFormParams = encodedFormParams;
}

void Query::setMultipartFormBody(QList<ParamValue> multipartFormParams)
{
    m_multipartFormParams = multipartFormParams;
}

void Query::setRawBody(QueryRawBody &rawBody)
{
    m_queryRawBody = rawBody;
}

void Query::setBinaryBody(QueryBinaryBody &binaryBody)
{
    m_queryBinaryBody = binaryBody;
}

void Query::setDeletedParameters(QList<int> &deletedParams)
{
    m_deletedParams = deletedParams;
}

void Query::setDeletedHeaders(QList<int> &deletedHeaders)
{
    m_deletedHeaders = deletedHeaders;
}

void Query::setDeletedMultipartParams(QList<int> &deletedMultipartParams)
{
    m_deletedMultiPartFormParams = deletedMultipartParams;
}

void Query::setDeletedEncodedFormParams(QList<int> &deletedEncodedFormParams)
{
    m_deletedEncodedFormParams = deletedEncodedFormParams;
}

Query::BodyType &Query::bodyType()
{
    return m_bodyType;
}

QString Query::bodyTypeToString(const BodyType bodyType)
{
    QString result;

    switch (bodyType) {
    case BodyType::Empty:
        result = "None";
        break;
    case Query::BodyType::Raw:
        result = "Raw";
        break;
    case Query::BodyType::EncodedForm:
        result = "Encoded";
        break;
    case Query::BodyType::MultipartForm:
        result = "Multipart";
        break;
    case Query::BodyType::Binary:
        result = "Binary";
        break;
    default:
        break;
    }

    return result;
}

Query::BodyType Query::bodyTypeFromString(const QString &bodyType)
{
    if (bodyType == "Raw")
    {
        return BodyType::Raw;
    }
    else if (bodyType == "Encoded")
    {
        return BodyType::EncodedForm;
    }
    else if (bodyType == "Multipart")
    {
        return BodyType::MultipartForm;
    }
    else if (bodyType == "Binary")
    {
        return BodyType::Binary;
    }
    else
    {
        return BodyType::Empty;
    }
}
