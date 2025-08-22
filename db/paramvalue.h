#ifndef PARAMVALUE_H
#define PARAMVALUE_H

#include <QString>
#include <QMap>
#include <QVariant>

class ParamValue
{

public:
    enum ParamValueType { String, File };

    static QString paramValueTypeToString(ParamValueType paramValueType);
    static ParamValueType paramValueTypeFromString(const QString &paramValueType);

    explicit ParamValue(const QMap<QString, QString> &valueMap);
    explicit ParamValue(std::optional<int> id, QString name, QString value, QString description);
    explicit ParamValue(int id, const QMap<QString, QString> &valueMap);

    std::optional<int> id();
    void setId(int id);

    ParamValueType getValueType();
    void setValueType(ParamValueType type);

    QString &getValue(const QString &key);
    void insert(QString key, QString value);

    void setAllValues(const QMap<QString, QString> &valueMap);
    QMap<QString, QString> &getAllValues();

private:
    ParamValueType m_type = ParamValueType::String;
    std::optional<int> m_id;
    QMap<QString, QString> m_valueMap;
    QString m_emptyResult = "";

};

#endif // PARAMVALUE_H
