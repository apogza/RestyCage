#ifndef QUERYSERIALIZER_H
#define QUERYSERIALIZER_H

#include "db/query.h"

#include <QObject>
#include <QJsonObject>
#include <QStandardItemModel>

class QuerySerializer : public QObject
{
    Q_OBJECT
public:
    explicit QuerySerializer(Query *query, QObject *parent = nullptr);
    void saveToFile(const QString &path);
    void addParameterArray(const QString &parameterName, QList<ParamValue> &parameters);
    void addParameter(const QString &parameterName, const QString &value);
    void addBasicAuth();
    void addBearerAuth();
    void addRawBody();
    void addMultipartFormBody();
    void addEncodedFormBody();
    void addBinaryBody();

private:
    QJsonObject m_jsonObject;
    Query *m_query;
    QJsonDocument createJsonDocument();

};

#endif // QUERYSERIALIZER_H
