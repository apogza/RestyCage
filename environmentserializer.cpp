#include "environmentserializer.h"

#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

EnvironmentSerializer::EnvironmentSerializer(QObject *parent)
    : QObject{parent}
{}

void EnvironmentSerializer::addVariable(QString name, QString value, QString description)
{
    QJsonObject var;
    var.insert("name", name);
    var.insert("value", value);
    var.insert("description", description);

    jsonArray.append(var);
}

void EnvironmentSerializer::saveToFile(QString fileName)
{
    QJsonDocument jsonDocument;
    jsonDocument.setArray(jsonArray);
    QDir envDir("envs");

    bool directoryCreated = true;
    if (!envDir.exists())
    {
        directoryCreated = QDir().mkdir("envs");
    }

    if (!directoryCreated)
    {
        return;
    }

    QFile jsonFile(QString("%1/%2").arg("envs", fileName));

    if (jsonFile.exists())
    {
        jsonFile.remove();
    }

    jsonFile.open(QFile::WriteOnly);
    jsonFile.write(jsonDocument.toJson());
}
