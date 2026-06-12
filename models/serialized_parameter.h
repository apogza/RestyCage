#ifndef SERIALIZED_PARAMETER_H
#define SERIALIZED_PARAMETER_H

#include <QObject>
#include <QString>

class SerializedParameter : public QObject
{
    Q_OBJECT
public:
    explicit SerializedParameter(QObject *parent = nullptr);
    explicit SerializedParameter(const QString &name, const QString &value, const QString &description, QObject *parent = nullptr);
    const QString name();
    const QString value();
    const QString description();



private:

    QString m_name;
    QString m_value;
    QString m_description;


};

#endif // SERIALIZED_PARAMETER_H
