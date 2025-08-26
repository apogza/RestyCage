#ifndef QUERY_H
#define QUERY_H

#include <QString>
#include <QList>

#include "paramvalue.h"
#include "query_auth.h"


class Query
{

public:
    enum BodyType { Empty, EncodedForm, MultipartForm, Raw, Binary };
    enum RawBodyType { JSON, Plain, XML, HTML, JavaScript };
    Query();
    Query(std::optional<int> id, QString name);

    std::optional<int> id();
    std::optional<int> collectionId();
    QString &name();
    QString &method();
    QString &url();
    QueryAuth &auth();

    BodyType &bodyType();
    QString &rawBodyValue();

    QList<ParamValue> &parameters();
    QList<ParamValue> &headers();
    QList<ParamValue> &encodedForm();
    QList<ParamValue> &multipartForm();
    QString &binaryForm();

    void setId(int id);
    void setCollectionId(int collectionId);
    void setName(QString name);
    void setMethod(QString method);
    void setUrl(QString url);

    void setAuth(QueryAuth auth);
    void setBodyType(BodyType bodyType);
    void setRawBodyType(RawBodyType rawBodyType);
    void setRawBodyValue(QString rawBodyValue);

    void setParameters(QList<ParamValue> parameters);
    void setHeaders(QList<ParamValue> headers);
    void setEncodedForm(QList<ParamValue> encodedFormParams);
    void setMultipartForm(QList<ParamValue> multipartFormParams);
    void setBinaryForm(QString binaryFilePath);

    static QString bodyTypeToString(const BodyType bodyType);
    static QString rawBodyToString(const RawBodyType rawBodyType);

    static BodyType bodyTypeFromString(const QString &bodyType);
    static RawBodyType rawBodyTypeFromString(const QString &rawBodyType);

private:
    std::optional<int> m_id;
    int m_collectionId;
    QString m_name;
    QString m_method;
    QString m_url;

    QueryAuth m_auth;

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
