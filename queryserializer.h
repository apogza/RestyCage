#ifndef QUERYSERIALIZER_H
#define QUERYSERIALIZER_H

#include <QObject>
#include <QJsonObject>
#include <QStandardItemModel>

class QuerySerializer : public QObject
{
    Q_OBJECT
public:
    explicit QuerySerializer(QObject *parent = nullptr);
    void saveToFile(const QString &path, const QString &fileName);
    void addParameterArray(const QString &parameterName, const QStandardItemModel &parameters, const int numColumns);
    void addParameter(const QString &parameterName, const QString &value);
    void addBasicAuth(const QString &username, const QString &password);
    void addBearerAuth(const QString &bearerToken);
    void addBody(const QString &type, const QString &body);

private:
    QJsonObject jsonObject;
};

#endif // QUERYSERIALIZER_H
