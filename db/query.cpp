#include "query.h"


Query::AuthType Query::authTypeFromString(const QString &authType)
{
    if (authType == "Bearer")
    {
        return AuthType::BearerToken;
    }
    else if (authType == "Basic")
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
        result = "Basic";
        break;
    case AuthType::BearerToken:
        result = "Bearer";
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

BasicQueryAuth &Query::basicAuth()
{
    return m_basicAuth;
}

BearerQueryAuth &Query::bearerAuth()
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

QList<ParamValue> &Query::encodedForm()
{
    return m_encodedFormParams;
}

QList<ParamValue> &Query::multipartForm()
{
    return m_multipartFormParams;
}

QString &Query::binaryForm()
{
    return m_binaryFilePath;
}

void Query::setId(int id)
{
    m_id = id;
}

void Query::setCollectionId(int collectionId)
{
    m_collectionId = collectionId;
}

void Query::setName(QString name)
{
    m_name = name;
}

void Query::setMethod(QString method)
{
    m_method = method;
}

void Query::setUrl(QString url)
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

void Query::setRawBodyType(RawBodyType rawBodyType)
{
    m_rawBodyType = rawBodyType;
}

void Query::setRawBodyValue(QString rawBodyValue)
{
    m_rawBodyValue = rawBodyValue;
}

void Query::setParameters(QList<ParamValue> parameters)
{
    m_parameters = parameters;
}

void Query::setHeaders(QList<ParamValue> headers)
{
    m_headers = headers;
}

void Query::setEncodedForm(QList<ParamValue> encodedFormParams)
{
    m_encodedFormParams = encodedFormParams;
}

void Query::setMultipartForm(QList<ParamValue> multipartFormParams)
{
    m_multipartFormParams = multipartFormParams;
}

void Query::setBinaryForm(QString binaryFilePath)
{
    m_binaryFilePath = binaryFilePath;
}

QString &Query::rawBodyValue()
{
    return m_rawBodyValue;
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

QString Query::rawBodyToString(const RawBodyType rawBodyType)
{
    QString result;

    switch (rawBodyType){
    case RawBodyType::JSON:
        result = "JSON";
        break;
    case RawBodyType::Plain:
        result = "Plain";
        break;

    case RawBodyType::XML:
        result = "XML";
        break;
    case RawBodyType::HTML:
        result = "HTML";
        break;
    case RawBodyType::JavaScript:
        result = "JavaScript";
        break;
    }

    return result;
}

Query::RawBodyType Query::rawBodyTypeFromString(const QString &rawBodyType)
{
    if (rawBodyType == "JSON")
    {
        return RawBodyType::JSON;
    }
    else if (rawBodyType == "Plain")
    {
        return RawBodyType::Plain;
    }
    else if (rawBodyType == "XML")
    {
        return RawBodyType::XML;
    }
    else if (rawBodyType == "HTML")
    {
        return RawBodyType::HTML;
    }
    else
    {
        return RawBodyType::JavaScript;
    }
}

