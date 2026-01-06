#include "network_helper.h"
#include "constants.h"


#include <QDateTime>
#include <QFile>
#include <QHttpMultiPart>
#include <QNetworkReply>
#include <QSslConfiguration>


NetworkHelper::NetworkHelper(QObject *parent)
    : QObject{parent}
{
    m_nam = new QNetworkAccessManager(this);
    m_nam->setStrictTransportSecurityEnabled(false);

    QSslConfiguration sslConf = QSslConfiguration::defaultConfiguration();
    sslConf.setPeerVerifyMode(QSslSocket::VerifyNone);
    QSslConfiguration::setDefaultConfiguration(sslConf);
}

void NetworkHelper::setRequestsUrlParams(QUrlQuery &url, QList<ParamValue> &params)
{
    for (ParamValue &param : params)
    {
        url.addQueryItem(param.value("name"), param.value("value"));
    }
}

void NetworkHelper::setRequestBasicAuth(QString &username, QString &password)
{
    m_request.value().setRawHeader(authorizationHeader, QString("Basic %1:%2").arg(username, password).toUtf8());
}

void NetworkHelper::setRequestBearerAuth(QString &bearerToken)
{
    m_request.value().setRawHeader(authorizationHeader, QString("Bearer %1").arg(bearerToken).toUtf8());
}

void NetworkHelper::setRequestHeaders(QList<ParamValue> &headers)
{
    for (ParamValue &param : headers)
    {
        m_request.value().headers().append(param.value("name"), param.value("value"));
    }
}

void NetworkHelper::initRequest(QUrl &url)
{
    m_request = QNetworkRequest(url);
}

void NetworkHelper::setRequestUrlQUery(QUrlQuery &urlQuery)
{

}

void NetworkHelper::sendMultiPartRequest(const QString &method, QList<ParamValue> &paramValues)
{
    m_request.value().setHeader(QNetworkRequest::ContentTypeHeader, QVariant("multipart/form-data"));
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    for (ParamValue &paramValue : paramValues)
    {
        QString key = paramValue.value("name");
        QString type = paramValue.value("type");
        QString value = paramValue.value("value");

        QHttpPart part;
        part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString("form-data; name=\"%1\"").arg(key)));

        if (type == "File")
        {
            part.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
            QFile *file = new QFile(value, multiPart);

            if (file->open(QIODevice::ReadOnly))
            {
                part.setBodyDevice(file);
                multiPart->append(part);
            }
        }
        else
        {
            part.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
            part.setBody(value.toUtf8());
        }
    }

    m_requestStartMs = QDateTime::currentMSecsSinceEpoch();

    m_reply = m_nam->sendCustomRequest(m_request.value(), method.toUtf8(), multiPart);
    connect(m_reply, &QNetworkReply::finished, this, &NetworkHelper::readReply);
}

void NetworkHelper::sendUrlEncodedFormRequest(const QString &method, QUrlQuery &urlQuery, QList<ParamValue> &paramValues)
{
    for (ParamValue &param: paramValues)
    {
        urlQuery.addQueryItem(param.value("name"), param.value("value"));
    }

    m_requestStartMs = QDateTime::currentMSecsSinceEpoch();

    m_reply = m_nam->sendCustomRequest(m_request.value(), method.toUtf8());
    connect(m_reply, &QNetworkReply::finished, this, &NetworkHelper::readReply);
}

void NetworkHelper::sendRawRequest(const QString &method, const QString &bodyType, const QByteArray &body)
{
    if (bodyType == "JSON")
    {
        m_request.value().setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    }
    else if (bodyType == "XML")
    {
        m_request.value().setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/xml"));
    }
    else if (bodyType == "HTML")
    {
        m_request.value().setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/html"));
    }
    else if (bodyType == "JavaScript")
    {
        m_request.value().setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/javascript"));
    }
    else
    {
        m_request.value().setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    }

    m_requestStartMs = QDateTime::currentMSecsSinceEpoch();

    m_reply = m_nam->sendCustomRequest(m_request.value(), method.toUtf8(), body);
    connect(m_reply, &QNetworkReply::finished, this, &NetworkHelper::readReply);
}

void NetworkHelper::sendBinaryRequest(const QString &method, QString &filePath)
{
    m_request.value().setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));

    QFile file(filePath);

    QByteArray fileByteArray;
    if (file.open(QIODevice::ReadOnly))
    {
        fileByteArray = file.readAll();
    }

    m_requestStartMs = QDateTime::currentMSecsSinceEpoch();

    m_reply = m_nam->sendCustomRequest(m_request.value(), method.toUtf8(), fileByteArray);

    connect(m_reply, &QNetworkReply::finished, this, &NetworkHelper::readReply);
}

void NetworkHelper::sendCustomRequest(const QString &method)
{
    QNetworkRequest req = m_request.value();

    m_reply = m_nam->sendCustomRequest(req, method.toUtf8());

    m_requestStartMs = QDateTime::currentMSecsSinceEpoch();

    connect(m_reply, &QNetworkReply::finished, this, &NetworkHelper::readReply);
}

void NetworkHelper::cancelRequest()
{
    if (m_reply && m_reply->isRunning())
    {
        m_reply->abort();
    }
}

int NetworkHelper::statusCode()
{
    return m_statusCode;
}

qint64 NetworkHelper::replyTotalTime()
{
    return m_totalTime;
}

QByteArray &NetworkHelper::replyBody()
{
    return m_replyBody;
}

QMap<QString, QString> &NetworkHelper::replyHeaders()
{
    return m_replyHeaders;
}

QString NetworkHelper::replyType() const
{
    return m_replyType;
}

void NetworkHelper::readReply()
{    
    readReplyHeaders(m_reply);

    m_statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    m_replyType = m_reply->header(QNetworkRequest::ContentTypeHeader).toString();

    m_replyBody = m_reply->readAll();

    m_totalTime = QDateTime::currentMSecsSinceEpoch() - m_requestStartMs;

    m_request = std::nullopt;
    emit replyReceived();
}

void NetworkHelper::readReplyHeaders(QNetworkReply *reply)
{
    QSet<QString> headerSet;
    QHttpHeaders replyHeaders = reply->headers();

    m_replyHeaders.clear();

    int j = 0;

    for (int i = 0; i < replyHeaders.size(); i++)
    {
        QString headerKey = replyHeaders.nameAt(i);

        /*
        if (headerSet.contains(headerKey))
        {
            continue;
        }

        headerSet.insert(headerKey);
        */

        m_replyHeaders.insert(headerKey, replyHeaders.valueAt(i).toByteArray());
    }
}
