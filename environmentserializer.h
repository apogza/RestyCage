#ifndef ENVIRONMENTSERIALIZER_H
#define ENVIRONMENTSERIALIZER_H

#include <QObject>
#include <QJsonArray>
#include <QStandardItemModel>

class EnvironmentSerializer : public QObject
{
    Q_OBJECT
public:
    explicit EnvironmentSerializer(QObject *parent = nullptr);
    void addVariable(QString name, QString value, QString description);
    void saveToFile(const QString &fileName);
    void loadModelFromFile(const QString &fileName, QStandardItemModel &model);

private:
    QJsonArray jsonArray;

};

#endif // ENVIRONMENTSERIALIZER_H
