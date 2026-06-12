#include "serialized_authentication.h"

SerializedAuthentication::SerializedAuthentication(QObject *parent)
    : QObject{parent}
{}

SerializedAuthentication::SerializedAuthentication(QString &username, QString &password, QObject *parent)
    : SerializedAuthentication(parent)
{
    m_username = username;
    m_password = password;
    m_authType = AuthType::Basic;
}

SerializedAuthentication::SerializedAuthentication(QString &bearerToken, QObject *parent)
    : SerializedAuthentication(parent)
{
    m_bearerToken = bearerToken;
    m_authType = AuthType::BearerToken;

}
