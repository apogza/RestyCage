#ifndef SERIALIZED_QUERY_H
#define SERIALIZED_QUERY_H
#include <QObject>
#include <QString>
#include <QList>
#include "serialized_parameter.h"

class SerializedQuery : public QObject
{
    Q_OBJECT

public:
    SerializedQuery();
    void setParams(QList<SerializedParameter> &params);

private:
    QString m_title;
    QString m_url;
    QString m_method;
    QList<SerializedParameter> m_params;



};



#endif // SERIALIZED_QUERY_H
