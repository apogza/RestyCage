#include "valuestore.h"

ValueStore::ValueStore(QObject *parent)
    : QObject {parent}
{
}

ValueStore::ValueStore(QString key, QString value, QString description, QObject *parent)
    :ValueStore(parent)
{
    m_key = key;
    m_value = value;
    m_description = description;
}

ValueStore::ValueStore(QString key, ValueStoreType type, QString value, QString description, QObject *parent)
    :ValueStore(key, value, description, parent)
{
    m_type = type;
}

void ValueStore::setValueType(ValueStoreType type)
{
    m_type = type;
}

void ValueStore::setKey(QString key)
{
    m_key = key;
}

void ValueStore::setValue(QString value)
{
    m_value = value;
}

void ValueStore::setDescription(QString description)
{
    m_description = description;
}

QString &ValueStore::key()
{
    return m_key;
}

QString &ValueStore::value()
{
    return m_value;
}

QString &ValueStore::description()
{
    return m_description;
}
