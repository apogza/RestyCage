#ifndef VALUESTORE_H
#define VALUESTORE_H

#include <QObject>
#include <QString>

class ValueStore : QObject
{
    Q_OBJECT


public:

    enum ValueStoreType { String, File };

    explicit ValueStore(QObject *parent = nullptr);

    ValueStore(QString key, QString value, QString description, QObject *parent = nullptr);
    ValueStore(QString key, ValueStoreType type, QString value, QString description, QObject *parent = nullptr);

    void setValueType(ValueStoreType type);
    void setKey(QString key);
    void setValue(QString value);
    void setDescription(QString descriptiption);

    QString &key();
    QString &value();
    QString &description();

private:
    ValueStoreType m_type;
    QString m_key;
    QString m_value;
    QString m_description;

};

#endif // VALUESTORE_H
