#include "query.h"

Query::Query(QObject *parent)
    : QObject{parent}
{

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

QString &Query::username()
{
    return m_username;
}

QString &Query::password()
{
    return m_password;
}

QString &Query::bearerToken()
{
    return m_bearerToken;
}

QList<ValueStore> &Query::parameters()
{
    return m_parameters;
}

QList<ValueStore> &Query::headers()
{
    return m_headers;
}

QList<ValueStore> &Query::encodedForm()
{
    return m_encodedForm;
}

QList<ValueStore> &Query::multipartForm()
{
    return m_multipartForm;
}


Query::RawBodyType Query::rawBodyValue()
{
    return m_rawBodyType;
}

Query::BodyType &Query::bodyType()
{
    return m_bodyType;
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

Query::AuthType Query::authType()
{
    return m_authType;
}
