#include "queryserializer.h"
#include "constants.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDir>

QuerySerializer::QuerySerializer(QObject *parent)
    : QObject{parent}
{
}

void QuerySerializer::saveToFile(const QString &path, const QString &fileName)
{
    QJsonDocument jsonDocument;
    jsonDocument.setObject(jsonObject);

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

    QFile jsonFile(QString("%1/%2").arg(path, fileName));
    jsonFile.open(QFile::WriteOnly);
    jsonFile.write(jsonDocument.toJson());
}

void QuerySerializer::addParameterArray(const QString &parameterName, const QStandardItemModel &parameters, int numColumns)
{
    QJsonArray parametersArray;
    for (int i = 0; i < parameters.rowCount(); i++)
    {
        QJsonObject jsonObject;
        for (int j = 0; j < numColumns; j++)
        {
            QVariant headerData = parameters.headerData(j, Qt::Orientation::Horizontal, Qt::DisplayRole);

            jsonObject.insert(
                headerData.toString(),
                parameters.item(i, j)->data(Qt::DisplayRole).toString());
        }

        parametersArray.append(jsonObject);
    }

    jsonObject.insert(parameterName, parametersArray);
}

void QuerySerializer::addParameter(const QString &parameterName, const QString &value)
{
    jsonObject.insert(parameterName, value);
}

void QuerySerializer::addBasicAuth(const QString &username, const QString &password)
{
    QJsonObject authObject;

    authObject.insert(querySerializationType, querySerializationBasic);
    authObject.insert(querySerializationUsername, username);
    authObject.insert(querySerializationPassword, password);

    jsonObject.insert(querySerializationAuthorization, authObject);
}

void QuerySerializer::addBearerAuth(const QString &bearerToken)
{
    QJsonObject authObject;

    authObject.insert(querySerializationType, querySerializationBearer);
    authObject.insert(querySerializationBearer, bearerToken);

    jsonObject.insert(querySerializationAuthorization, authObject);
}

void QuerySerializer::addBody(const QString &type, const QString &body)
{
    QJsonObject bodyObject;
    bodyObject.insert(querySerializationType, type);
    bodyObject.insert(querySerializationValue, body);

    jsonObject.insert(querySerializationBody, bodyObject);
}
