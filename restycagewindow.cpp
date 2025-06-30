#include "restycagewindow.h"
#include "ui_restycagewindow.h"
#include "QNetworkReply"
#include "QJsonDocument"

RestyCageWindow::RestyCageWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RestyCageWindow)
{
    ui->setupUi(this);
    nam = new QNetworkAccessManager(this);
}

RestyCageWindow::~RestyCageWindow()
{
    delete ui;
}

void RestyCageWindow::on_sendButton_clicked()
{
    QUrl reqUrl(ui->urlEdit->text());
    QNetworkRequest req(reqUrl);

    setRequestParams(req);
    setRequestAuth(req);
    setRequestBody(req);

    sendRequest(req);
}


void RestyCageWindow::on_comboBox_currentIndexChanged(int index)
{
    ui->authStackedWidget->setCurrentIndex(index);
}

void RestyCageWindow::setRequestParams(QNetworkRequest &req)
{

}

void RestyCageWindow::setRequestAuth(QNetworkRequest &req)
{
    if (ui->authComboBox->currentText() == "Bearer Token")
    {
        QString bearerToken = ui->bearerTokenEdit->text();
        req.setRawHeader("Authorization",  QString("Bearer %1").arg(bearerToken).toUtf8());
    }
}

void RestyCageWindow::setRequestBody(QNetworkRequest &req)
{

}

void RestyCageWindow::sendRequest(QNetworkRequest &request)
{
    const QString method = ui->methodComboBox->currentText();
    QNetworkReply *reply = nam->sendCustomRequest(request, method.toUtf8());
    connect(reply, &QNetworkReply::finished, this, &RestyCageWindow::readReply);
}

void RestyCageWindow::readReply()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    QByteArray body = reply->readAll();
    QJsonDocument document = QJsonDocument::fromJson(body);

    ui->respBodyLbl->setText(document.toJson());
}


void RestyCageWindow::on_authComboBox_currentIndexChanged(int index)
{
    ui->authStackedWidget->setCurrentIndex(index);
}

