#include "paramvalue.h"


QString ParamValue::paramValueTypeToString(ParamValueType paramValueType)
{
    switch (paramValueType)
    {
    case ParamValueType::File:
        return "file";
    case ParamValueType::String:
    default:
        return "string";
    }
}

ParamValue::ParamValueType ParamValue::paramValueTypeFromString(const QString &paramValueType)
{
    if (paramValueType == "file")
    {
        return ParamValueType::File;
    }
    else
    {
        return ParamValueType::String;
    }
}

ParamValue::ParamValue(const QMap<QString, QString> &valueMap)
{
    setAllValues(valueMap);
}

ParamValue::ParamValue(int id, const QMap<QString, QString> &valueMap)
    : ParamValue(valueMap)
{
    m_id.setValue(id);
}

QVariant &ParamValue::id()
{
    return m_id;
}

void ParamValue::setId(int id)
{
    m_id.setValue(id);
}

ParamValue::ParamValueType ParamValue::getValueType()
{
    return m_type;
}

void ParamValue::setValueType(ParamValueType type)
{
    m_type = type;
}

QString &ParamValue::getValue(const QString &key)
{
    if (m_valueMap.contains(key))
    {
        return m_valueMap[key];
    }

    return m_emptyResult;
}

void ParamValue::insert(QString key, QString value)
{
    m_valueMap.insert(key, value);
}

void ParamValue::setAllValues(const QMap<QString, QString> &valueMap)
{
    QMapIterator<QString, QString> it(valueMap);

    while(it.hasNext())
    {
        auto item = it.next();
        m_valueMap.insert(item.key(), item.value());
    }
}

QMap<QString, QString> &ParamValue::getAllValues()
{
    return m_valueMap;
}

