#include "environmentserializer.h"
#include "constants.h"

#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardItem>

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

void EnvironmentSerializer::saveToFile(const QString &fileName)
{
    QJsonDocument jsonDocument;
    jsonDocument.setArray(jsonArray);
    QDir envDir(envDirPath);

    bool directoryCreated = true;
    if (!envDir.exists())
    {
        directoryCreated = QDir().mkdir(envDirPath);
    }

    if (!directoryCreated)
    {
        return;
    }

    QFile jsonFile(QString("%1/%2.json").arg(envDirPath, fileName));

    if (jsonFile.exists())
    {
        jsonFile.remove();
    }

    jsonFile.open(QFile::WriteOnly);
    jsonFile.write(jsonDocument.toJson());
}

void EnvironmentSerializer::loadModelFromFile(const QString &jsonFileName, QStandardItemModel &model)
{
    QFile file(QString("%1/%2.json").arg(envDirPath, jsonFileName));

    file.open(QFile::ReadOnly);

    QJsonDocument jsonDocument = QJsonDocument::fromJson(file.readAll());
    QJsonArray jsonVarArray = jsonDocument.array();

    int row = 0;
    foreach (const QJsonValue &val, jsonVarArray)
    {
        QJsonObject jsonObject = val.toObject();

        QStandardItem *nameItem = new QStandardItem();
        nameItem->setText(jsonObject["name"].toString());
        model.setItem(row, 0, nameItem);

        QStandardItem *valueItem = new QStandardItem();
        valueItem->setText(jsonObject["value"].toString());
        model.setItem(row, 1, valueItem);

        QStandardItem *descriptionItem = new QStandardItem();
        descriptionItem->setText(jsonObject["description"].toString());
        model.setItem(row, 2, descriptionItem);

        row++;
    }
}
