#include "queryserializer.h"
#include "constants.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDir>

QuerySerializer::QuerySerializer(Query *query, QObject *parent)
    : QObject{parent}
{
    m_query = query;
}

void QuerySerializer::saveToFile(const QString &path)
{
    QJsonDocument jsonDocument = createJsonDocument();

    QDir queryDir(collectionDirPath);

    bool directoryCreated = true;
    if (!queryDir.exists())
    {
        directoryCreated = QDir().mkdir(collectionDirPath);
    }

    if (!directoryCreated)
    {
        return;
    }

    QString filename = QString("%1.json").arg(m_query->name());

    QFile jsonFile(QString("%1/%2").arg(path, QString("%1.json").arg(filename)));
    jsonFile.open(QFile::WriteOnly);
    jsonFile.write(jsonDocument.toJson());
}

void QuerySerializer::addParameterArray(const QString &parameterName, QList<ParamValue> &paramValues)
{
    QJsonArray parametersArray;

    for (int i = 0; i < paramValues.count(); i++)
    {
        QJsonObject jsonObject;

        QMap<QString, QString> paramMap = paramValues[i].getAllValues();
        QMapIterator iter(paramMap);

        while(iter.hasNext())
        {
            auto kvp = iter.next();
            jsonObject.insert(kvp.key(), kvp.value());
        }

        parametersArray.append(jsonObject);
    }

    m_jsonObject.insert(parameterName, parametersArray);
}

void QuerySerializer::addParameter(const QString &parameterName, const QString &value)
{
    m_jsonObject.insert(parameterName, value);
}

void QuerySerializer::addBasicAuth()
{
    QJsonObject authObject;

    authObject.insert(querySerializationType, querySerializationBasic);
    authObject.insert(querySerializationUsername, m_query->username());
    authObject.insert(querySerializationPassword, m_query->password());

    m_jsonObject.insert(querySerializationAuthorization, authObject);
}

void QuerySerializer::addBearerAuth()
{
    QJsonObject authObject;

    authObject.insert(querySerializationType, querySerializationBearer);
    authObject.insert(querySerializationBearer, m_query->bearerToken());

    m_jsonObject.insert(querySerializationAuthorization, authObject);
}

void QuerySerializer::addRawBody()
{
    QJsonObject bodyObject;
    bodyObject.insert(querySerializationType, Query::bodyTypeToString(m_query->bodyType()));
    bodyObject.insert(querySerializationValue, m_query->rawBodyValue());

    m_jsonObject.insert(querySerializationBody, bodyObject);
}

void QuerySerializer::addMultipartFormBody()
{
    QJsonObject bodyObject;
    bodyObject.insert(querySerializationType, Query::bodyTypeToString(m_query->bodyType()));

    QJsonArray formDataArray;
    QList<ParamValue> params = m_query->multipartForm();

    for (int i = 0; i < params.size(); i++)
    {
        QJsonObject paramObject;
        QMap<QString, QString> paramValues = params[i].getAllValues();
        QMapIterator<QString, QString> it(paramValues);

        while (it.hasNext())
        {
            auto item = it.next();
            paramObject.insert(item.key(), item.value());
        }

        paramObject.insert(querySerializationType, ParamValue::paramValueTypeToString(params[i].getValueType()));
        formDataArray.append(paramObject);
    }

    bodyObject.insert(querySerializationValue, formDataArray);
    m_jsonObject.insert(querySerializationBody, bodyObject);
}

void QuerySerializer::addEncodedFormBody()
{
    QJsonObject bodyObject;
    bodyObject.insert(querySerializationType, Query::bodyTypeToString(m_query->bodyType()));

    QJsonArray encodedFormArray;

    QList<ParamValue> params = m_query->encodedForm();

    for (int i = 0; i < params.size(); i++)
    {
        QJsonObject paramObject;

        QMap<QString, QString> paramValues = params[i].getAllValues();
        QMapIterator<QString, QString> it(paramValues);

        while(it.hasNext())
        {
            auto item = it.next();
            paramObject.insert(item.key(), item.value());
        }

        paramObject.insert(querySerializationType, ParamValue::paramValueTypeToString(ParamValue::ParamValueType::String));
        encodedFormArray.append(paramObject);
    }

    bodyObject.insert(querySerializationValue, encodedFormArray);
    m_jsonObject.insert(querySerializationBody, bodyObject);
}

void QuerySerializer::addBinaryBody()
{
    QJsonObject bodyObject;
    bodyObject.insert(querySerializationType, Query::bodyTypeToString(m_query->bodyType()));
    bodyObject.insert(querySerializationBody, m_query->binaryForm());

    m_jsonObject.insert(querySerializationValue, bodyObject);
}

QJsonDocument QuerySerializer::createJsonDocument()
{
    QJsonDocument jsonDocument;

    addParameter(querySerializationName, m_query->name());
    addParameter(querySerializationMethod, m_query->method());

    if (m_query->authType() == Query::AuthType::Basic)
    {
        addBasicAuth();
    }

    if (!m_query->parameters().empty())
    {
        addParameterArray(querySerializationParams, m_query->parameters());
    }

    if (!m_query->headers().empty())
    {
        addParameterArray(querySerializationHeaders, m_query->headers());
    }

    if (m_query->bodyType() == Query::BodyType::Raw)
    {
        addRawBody();
    }

    if (m_query->bodyType() == Query::BodyType::MultipartForm)
    {
        addMultipartFormBody();
    }

    if (m_query->bodyType() == Query::BodyType::EncodedForm)
    {
        addEncodedFormBody();
    }

    if (m_query->bodyType() == Query::BodyType::Binary)
    {
        addBinaryBody();
    }

    jsonDocument.setObject(m_jsonObject);

    return jsonDocument;
}
