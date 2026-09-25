#ifndef NETWORK_HELPER_H
#define NETWORK_HELPER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QUrlQuery>
#include "db/param_value.h"

class NetworkHelper : public QObject
{
    Q_OBJECT
public:
    explicit NetworkHelper(QObject *parent = nullptr);

    void setRequestsUrlParams(QUrlQuery &url, QList<ParamValue> &params);
    void setRequestBasicAuth(QString &username, QString &password);
    void setRequestBearerAuth(QString &bearerToken);
    void setRequestHeaders(QList<ParamValue> &headers);
    void initRequest(QUrl &urlQuery);

    void sendMultiPartRequest(const QString &method, QList<ParamValue> &paramValues);
    void sendUrlEncodedFormRequest(const QString &method, QUrlQuery &urlQuery, QList<ParamValue> &paramValues);
    void sendRawRequest(const QString &method, const QString &bodyType, const QByteArray &body);
    void sendBinaryRequest(const QString &method, QString &filePath);
    void sendCustomRequest(const QString &method);
    void cancelRequest();

    int statusCode();
    qint64 replyTotalTime();
    QByteArray &replyBody();
    QByteArray &partialReplyBody();
    QMap<QString, QString> &replyHeaders();
    QString replyType() const;

private:
    QNetworkAccessManager *m_nam;
    std::optional<QNetworkRequest> m_request;
    QNetworkReply *m_reply;

    qint64 m_requestStartMs;
    qint64 m_totalTime;
    int m_statusCode;
    QString m_replyType;

    QByteArray m_replyBody;
    QByteArray m_partialReplyBody;
    QMap<QString, QString> m_replyHeaders;

    void readReply();
    void readPartialReply();
    void readReplyHeaders(QNetworkReply *reply);

signals:
    void replyReceived();
    void partialReplyReceived();

};

#endif // NETWORK_HELPER_H
