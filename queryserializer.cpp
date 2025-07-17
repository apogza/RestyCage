#include "queryserializer.h"
#include <QFile>
#include <QJsonDocument>

QuerySerializer::QuerySerializer(QObject *parent)
    : QObject{parent}
{

}

void QuerySerializer::saveToFile(QString path, QString fileName)
{
    QJsonDocument jsonDocument;
    jsonDocument.setObject(jsonObject);

    QFile jsonFile(QString("%1/%2").arg(path, fileName));
    jsonFile.open(QFile::WriteOnly);
    jsonFile.write(jsonDocument.toJson());
}

void QuerySerializer::addName(QString name)
{
    jsonObject.insert("name", name);
}

void QuerySerializer::addUrl(QString url)
{
    jsonObject.insert("url", url);
}

void QuerySerializer::addMethod(QString method)
{
    jsonObject.insert("method", method);
}



