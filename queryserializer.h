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
    void saveToFile(QString path, QString fileName);
    void addName(QString name);
    void addUrl(QString url);
    void addMethod(QString method);
    void addParameters(QStandardItemModel &parameters);

private:
    QJsonObject jsonObject;
};

#endif // QUERYSERIALIZER_H
