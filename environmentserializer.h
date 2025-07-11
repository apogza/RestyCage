#ifndef ENVIRONMENTSERIALIZER_H
#define ENVIRONMENTSERIALIZER_H

#include <QObject>
#include <QJsonArray>


class EnvironmentSerializer : public QObject
{
    Q_OBJECT
public:
    explicit EnvironmentSerializer(QObject *parent = nullptr);
    void addVariable(QString name, QString value, QString description);
    void saveToFile(QString fileName);

private:
    QJsonArray jsonArray;

};

#endif // ENVIRONMENTSERIALIZER_H
