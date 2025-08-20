#ifndef PARAMVALUE_H
#define PARAMVALUE_H

#include <QString>
#include <QMap>

class ParamValue
{

public:
    enum ParamValueType { String, File };

    static QString paramValueTypeToString(ParamValueType paramValueType);
    static ParamValueType paramValueTypeFromString(const QString &paramValueType);

    explicit ParamValue(const QMap<QString, QString> &valueMap);

    ParamValueType getValueType();
    void setValueType(ParamValueType type);

    QString &getValue(const QString &key);
    void insert(QString key, QString value);

    void setAllValues(const QMap<QString, QString> &valueMap);
    QMap<QString, QString> &getAllValues();


private:
    ParamValueType m_type = ParamValueType::String;

    QMap<QString, QString> m_valueMap;
    QString m_emptyResult = "";
};

#endif // PARAMVALUE_H
