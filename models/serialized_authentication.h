#ifndef SERIALIZED_AUTHENTICATION_H
#define SERIALIZED_AUTHENTICATION_H

#include <QObject>
#include "../constants.h"

class SerializedAuthentication : public QObject
{
    Q_OBJECT
public:
    explicit SerializedAuthentication(QObject *parent = nullptr);
    explicit SerializedAuthentication(QString &username, QString &password, QObject *parent = nullptr);
    explicit SerializedAuthentication(QString &bearerToken, QObject *parent = nullptr);

signals:

private:
    AuthType m_authType;

    QString m_username;
    QString m_password;
    QString m_bearerToken;
};

#endif // SERIALIZED_AUTHENTICATION_H
