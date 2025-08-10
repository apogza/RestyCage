#ifndef QUERY_H
#define QUERY_H

#include <QObject>
#include <QString>
#include <QList>

#include "valuestore.h"
#include "queryserializer.h"

class Query : QObject
{
    Q_OBJECT;

public:
    enum AuthType { None, Basic, BearerToken };
    enum BodyType { Empty, EncodedForm, MultipartForm, Raw, Binary };
    enum RawBodyType { JSON, Plain, XML, HTML, JavaScript };

    Query(QObject *parent = nullptr);

    QString &name();
    QString &method();
    QString &url();
    AuthType authType();

    QString &username();
    QString &password();
    QString &bearerToken();
    BodyType &bodyType();
    RawBodyType rawBodyValue();

    QList<ValueStore> &parameters();
    QList<ValueStore> &headers();
    QList<ValueStore> &encodedForm();
    QList<ValueStore> &multipartForm();

    static QString authTypeToString(const AuthType authType);
    static QString bodyTypeToString(const BodyType bodyType);
    static QString rawBodyToString(const RawBodyType rawBodyType);

    static AuthType authTypeFromString(const QString &authType);
    static BodyType bodyTypeFromString(const QString &bodyType);
    static RawBodyType rawBodyTypeFromString(const QString &rawBodyType);

private:
    QString m_name;
    QString m_method;
    QString m_url;

    AuthType m_authType;
    QString m_username;
    QString m_password;
    QString m_bearerToken;

    BodyType m_bodyType;
    RawBodyType m_rawBodyType;
    QString m_rawBodyValue;

    QList<ValueStore> m_parameters;
    QList<ValueStore> m_headers;
    QList<ValueStore> m_encodedForm;
    QList<ValueStore> m_multipartForm;

    QuerySerializer m_querySerializer = QuerySerializer(this);

};

#endif // QUERY_H
