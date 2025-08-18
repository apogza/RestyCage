#ifndef QUERY_H
#define QUERY_H

#include <QString>
#include <QList>

#include "paramvalue.h"


class Query
{

public:
    enum AuthType { None, Basic, BearerToken };
    enum BodyType { Empty, EncodedForm, MultipartForm, Raw, Binary   };
    enum RawBodyType { JSON, Plain, XML, HTML, JavaScript };

    QString &name();
    QString &method();
    QString &url();
    AuthType authType();

    QString &username();
    QString &password();
    QString &bearerToken();
    BodyType &bodyType();
    QString &rawBodyValue();

    QList<ParamValue> &parameters();
    QList<ParamValue> &headers();
    QList<ParamValue> &encodedForm();
    QList<ParamValue> &multipartForm();
    QString &binaryForm();

    void setName(QString name);
    void setMethod(QString method);
    void setUrl(QString url);
    void setAuthType(AuthType authType);

    void setUsername(QString username);
    void setPassword(QString password);
    void setBearerToken(QString bearerToken);
    void setBodyType(BodyType bodyType);
    void setRawBodyType(RawBodyType rawBodyType);
    void setRawBodyValue(QString rawBodyValue);

    void setParameters(QList<ParamValue> parameters);
    void setHeaders(QList<ParamValue> headers);
    void setEncodedForm(QList<ParamValue> encodedFormParams);
    void setMultipartForm(QList<ParamValue> multipartFormParams);
    void setBinaryForm(QString binaryFilePath);

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

    QList<ParamValue> m_parameters;
    QList<ParamValue> m_headers;
    QList<ParamValue> m_encodedFormParams;
    QList<ParamValue> m_multipartFormParams;
    QString m_binaryFilePath;
};

#endif // QUERY_H
