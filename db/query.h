#ifndef QUERY_H
#define QUERY_H

#include <QString>
#include <QList>

#include "paramvalue.h"
#include "basic_query_auth.h"
#include "bearer_query_auth.h"
#include "query_raw_body.h"
#include "query_binary_body.h"

class Query
{

public:
    enum BodyType { Empty, EncodedForm, MultipartForm, Raw, Binary };
    enum AuthType { None, Basic, BearerToken };

    static AuthType authTypeFromString(const QString &authType);
    static QString authTypeToString(const AuthType authType);

    Query();
    Query(std::optional<int> id, QString &name);

    std::optional<int> id();
    std::optional<int> collectionId();
    QString &name();
    QString &method();
    QString &url();
    AuthType authType();
    BasicQueryAuth &basicAuth();
    BearerQueryAuth &bearerAuth();

    BodyType &bodyType();

    QList<ParamValue> &parameters();
    QList<ParamValue> &headers();
    QList<ParamValue> &encodedFormBody();
    QList<ParamValue> &multipartFormBody();
    QueryRawBody &rawBody();
    QueryBinaryBody &binaryBody();

    QList<int> &deletedParams();
    QList<int> &deletedHeaders();
    QList<int> &deletedMultipartParams();
    QList<int> &deletedEncodedFormParams();

    void setId(int id);
    void setCollectionId(int collectionId);
    void setName(QString name);
    void setMethod(QString method);
    void setUrl(QString url);

    void setAuthType(AuthType authType);
    void setBasicAuth(BasicQueryAuth& auth);
    void setBearerAuth(BearerQueryAuth& auth);

    void setBodyType(BodyType bodyType);

    void setParameters(QList<ParamValue> parameters);
    void setHeaders(QList<ParamValue> headers);
    void setEncodedFormBody(QList<ParamValue> encodedFormParams);
    void setMultipartFormBody(QList<ParamValue> multipartFormParams);
    void setRawBody(QueryRawBody &rawBody);
    void setBinaryBody(QueryBinaryBody &binaryBody);

    void setDeletedParameters(QList<int> &deletedParams);
    void setDeletedHeaders(QList<int> &deletedHeaders);
    void setDeletedMultipartParams(QList<int> &deletedMultipartParams);
    void setDeletedEncodedFormParams(QList<int> &deletedEncodedFormParams);

    static QString bodyTypeToString(const BodyType bodyType);
    static BodyType bodyTypeFromString(const QString &bodyType);

private:
    std::optional<int> m_id;
    int m_collectionId;
    QString m_name;
    QString m_method;
    QString m_url;
    AuthType m_authType;

    BasicQueryAuth m_basicAuth;
    BearerQueryAuth m_bearerAuth;

    BodyType m_bodyType;
    QueryRawBody m_queryRawBody;
    QueryBinaryBody m_queryBinaryBody;

    QList<ParamValue> m_parameters;
    QList<ParamValue> m_headers;
    QList<ParamValue> m_encodedFormParams;
    QList<ParamValue> m_multipartFormParams;

    QList<int> m_deletedParams;
    QList<int> m_deletedHeaders;
    QList<int> m_deletedMultiPartFormParams;
    QList<int> m_deletedEncodedFormParams;
};

#endif // QUERY_H
