#include "serialized_parameter.h"

SerializedParameter::SerializedParameter(QObject *parent)
    : QObject{parent}
{}

SerializedParameter::SerializedParameter(const QString &name, const QString &value, const QString &description, QObject *parent)
    : SerializedParameter(parent)
{
    m_name = name;
    m_value = value;
    m_description = description;
}


const QString SerializedParameter::name()
{
    return m_name;
}

const QString SerializedParameter::value()
{
    return m_value;
}

const QString SerializedParameter::description()
{
    return m_description;
}
