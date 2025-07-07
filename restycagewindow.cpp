#include "restycagewindow.h"
#include "ui_restycagewindow.h"
#include "qjsonmodel.h"
#include "QModelIndexList"
#include "QUrlQuery"
#include "QHttpHeaders"
#include "QList"
#include "QSet"
#include "QIODevice"
#include "keyvaluedialog.h"
#include "keyvaluefiletextdialog.h"

RestyCageWindow::RestyCageWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RestyCageWindow)
{
    ui->setupUi(this);

    nam = new QNetworkAccessManager(this);

    ui->respHeadersTableWidget->setColumnCount(2);
    ui->respHeadersTableWidget->setHorizontalHeaderLabels(QStringList() << "Key" << "Value");

    initModels();
}

RestyCageWindow::~RestyCageWindow()
{
    delete ui;
}

void RestyCageWindow::on_sendButton_clicked()
{
    QUrl url(ui->urlEdit->text());

    QUrlQuery urlQuery(url);
    setRequestParams(urlQuery);

    url.setQuery(urlQuery);
    QNetworkRequest req(url);

    setRequestAuth(req);
    setRequestHeaders(req);

    setRequestBody(req);

    sendRequest(req);
}

void RestyCageWindow::setRequestParams(QUrlQuery &url)
{
    for (int i = 0; i < reqParamsModel.rowCount(); i++)
    {
        QString key = reqParamsModel.item(i, 0)->text();
        QString value = reqParamsModel.item(i, 1)->text();
        url.addQueryItem(key, value);
    }
}

void RestyCageWindow::setRequestAuth(QNetworkRequest &req)
{
    if (ui->authComboBox->currentText() == "Bearer Token")
    {
        QString bearerToken = ui->bearerTokenEdit->text();
        req.setRawHeader("Authorization",  QString("Bearer %1").arg(bearerToken).toUtf8());
    }

    if (ui->authComboBox->currentText() == "Basic Auth")
    {
        QString user = ui->authBasicUserEdit->text();
        QString password = ui->authBasicPasswordEdit->text();

        req.setRawHeader("Authorization", QString("Basic %1:%2").arg(user, password).toUtf8());
    }
}

void RestyCageWindow::setRequestBody(QNetworkRequest &req)
{
    QString bodyType = ui->reqBodyTypeComboBox->currentText();

    if (bodyType == "Form Data")
    {

    }
    else if (bodyType == "X-www-encoded-form")
    {

    }
    else if (bodyType == "Raw")
    {

    }
    else if (bodyType == "Binary")
    {

    }
}

void RestyCageWindow::setRequestHeaders(QNetworkRequest &req)
{
    for (int i = 0; i < reqHeadersModel.rowCount(); i++)
    {
        QString headerKey = reqHeadersModel.item(i, 0)->text();
        QString headerValue = reqHeadersModel.item(i, 1)->text();

        req.headers().append(headerKey, headerValue);
    }
}

void RestyCageWindow::sendRequest(QNetworkRequest &request)
{
    const QString method = ui->methodComboBox->currentText();
    QNetworkReply *reply = nam->sendCustomRequest(request, method.toUtf8());
    requestStartMs = QDateTime::currentMSecsSinceEpoch();

    connect(reply, &QNetworkReply::finished, this, &RestyCageWindow::readReply);
}

void RestyCageWindow::readReply()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    readReplyHeaders(reply);
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    QByteArray body = reply->readAll();

    qint64 totalTime = QDateTime::currentMSecsSinceEpoch() -  requestStartMs;

    ui->statusLbl->setText(QString("HTTP %1").arg(QString::number(statusCode)));
    ui->sizeLbl->setText(QString("%1 bytes").arg(QString::number(body.size())));
    ui->timeLbl->setText(QString("%1 ms").arg(QString::number(totalTime)));

    QJsonModel *model = new QJsonModel(ui->respJsonTreeView);
    ui->respJsonTreeView->setModel(model);
    model->loadJson(body);
}

void RestyCageWindow::readReplyHeaders(QNetworkReply *reply)
{
    QSet<QString> headerSet;
    QHttpHeaders replyHeaders = reply->headers();

    ui->respHeadersTableWidget->clearContents();
    ui->respHeadersTableWidget->setRowCount(0);

    int j = 0;
    for (int i = 0; i < replyHeaders.size(); i++)
    {
        QString headerKey = replyHeaders.nameAt(i);

        if (headerSet.contains(headerKey))
        {
            continue;
        }

        headerSet.insert(headerKey);

        QTableWidgetItem *itemKey = new QTableWidgetItem();
        itemKey->setText(headerKey);

        QTableWidgetItem *itemValue = new QTableWidgetItem();
        itemValue->setText(QString::fromUtf8(replyHeaders.valueAt(i).toByteArray()));

        ui->respHeadersTableWidget->insertRow(j);

        ui->respHeadersTableWidget->setItem(j, 0, itemKey);
        ui->respHeadersTableWidget->setItem(j, 1, itemValue);
        j++;
    }
}

void RestyCageWindow::on_authComboBox_currentIndexChanged(int index)
{
    ui->authStackedWidget->setCurrentIndex(index);
}

void RestyCageWindow::initModels()
{

    reqParamsModel.insertColumns(0, 3);
    reqParamsModel.setHeaderData(0, Qt::Horizontal, QObject::tr("Key"));
    reqParamsModel.setHeaderData(1, Qt::Horizontal, QObject::tr("Value"));
    reqParamsModel.setHeaderData(2, Qt::Horizontal, QObject::tr("Description"));

    ui->reqParamsTableView->setModel(&reqParamsModel);

    reqHeadersModel.insertColumns(0, 3);
    reqHeadersModel.setHeaderData(0, Qt::Horizontal, QObject::tr("Key"));
    reqHeadersModel.setHeaderData(1, Qt::Horizontal, QObject::tr("Value"));
    reqHeadersModel.setHeaderData(2, Qt::Horizontal, QObject::tr("Description"));

    ui->reqHeadersTableView->setModel(&reqHeadersModel);

    reqFormBodyModel.insertColumns(0, 4);
    reqFormBodyModel.setHeaderData(0, Qt::Horizontal, QObject::tr("Key"));
    reqFormBodyModel.setHeaderData(1, Qt::Horizontal, QObject::tr("Type"));
    reqFormBodyModel.setHeaderData(2, Qt::Horizontal, QObject::tr("Value"));
    reqFormBodyModel.setHeaderData(3, Qt::Horizontal, QObject::tr("Description"));

    ui->reqBodyFormTableView->setModel(&reqFormBodyModel);

    reqUrlEncodedFormBodyModel.insertColumns(0, 3);
    reqUrlEncodedFormBodyModel.setHeaderData(0, Qt::Horizontal, QObject::tr("Key"));
    reqUrlEncodedFormBodyModel.setHeaderData(1, Qt::Horizontal, QObject::tr("Value"));
    reqUrlEncodedFormBodyModel.setHeaderData(2, Qt::Horizontal, QObject::tr("Description"));

    ui->reqUrlEncodedBodyTableView->setModel(&reqUrlEncodedFormBodyModel);
}

void RestyCageWindow::addSimpleModelRow(QStandardItemModel &itemsModel)
{
    KeyValueDialog *dialog = new KeyValueDialog(this);

    int result = dialog->exec();

    if (result == QDialog::Accepted)
    {
        QString key = dialog->getKey();
        QString value = dialog->getValue();
        QString description = dialog->getDescription();

        itemsModel.insertRow(itemsModel.rowCount(),
                             {
                              new QStandardItem(key),
                              new QStandardItem(value),
                              new QStandardItem(description)
                             }
        );
    }
}

void RestyCageWindow::editSimpleRow(QStandardItemModel &itemsModel, int row, int column)
{
    QStandardItem *keyItem = itemsModel.item(row, 0);
    QStandardItem *valueItem = itemsModel.item(row, 1);
    QStandardItem *descriptionItem = itemsModel.item(row, 2);

    QString key = keyItem->data(Qt::EditRole).toString();
    QString value = valueItem->data(Qt::EditRole).toString();
    QString description = descriptionItem->data(Qt::EditRole).toString();

    KeyValueDialog *keyValueDialog = new KeyValueDialog(this, key, value, description);
    int result = keyValueDialog->exec();

    if (result == QDialog::Accepted)
    {
        QStandardItem *item = itemsModel.item(row, 0);
        item->setData(keyValueDialog->getKey(), Qt::EditRole);

        item = itemsModel.item(row, 1);
        item->setData(keyValueDialog->getValue(), Qt::EditRole);

        item = itemsModel.item(row, 2);
        item->setData(keyValueDialog->getDescription(), Qt::EditRole);
    }
}

void RestyCageWindow::removeModelRow(QTableView* tableView, QStandardItemModel &itemsModel)
{
    QItemSelectionModel *selectionModel = tableView->selectionModel();
    QModelIndexList indexlist = selectionModel->selectedIndexes();

    foreach (QModelIndex idx, indexlist)
    {
        itemsModel.removeRow(idx.row());
    }
}

void RestyCageWindow::on_reqAddParamBtn_clicked()
{
    addSimpleModelRow(reqParamsModel);
}

void RestyCageWindow::on_reqRemoveParamBtn_clicked()
{
    removeModelRow(ui->reqParamsTableView, reqParamsModel);
}

void RestyCageWindow::on_reqHeadersAddBtn_clicked()
{
    addSimpleModelRow(reqHeadersModel);
}

void RestyCageWindow::on_reqHeadersRemoveBtn_clicked()
{
    removeModelRow(ui->reqHeadersTableView, reqHeadersModel);
}

void RestyCageWindow::on_reqBodyTypeComboBox_currentIndexChanged(int index)
{
    ui->reqBodyStackedWidget->setCurrentIndex(index);
}

void RestyCageWindow::on_reqParamsTableView_doubleClicked(const QModelIndex &index)
{
    editSimpleRow(reqParamsModel, index.row(), index.column());
}

void RestyCageWindow::on_reqBodyFormDataAddRowBtn_clicked()
{
    KeyValueFileTextDialog *dialog = new  KeyValueFileTextDialog(this);

    int result = dialog->exec();
    if (result == QDialog::Accepted)
    {
        QString key = dialog->getKey();
        QString type = dialog->getType();
        QString filePath = dialog->getFilePathValue();
        QString fileName = dialog->getFileNameValue();
        QString textValue = dialog->getTextValue();

        QString description = dialog->getDescription();

        QStandardItem *keyItem = new QStandardItem();
        keyItem->setData(key, Qt::EditRole);

        QStandardItem *typeItem = new QStandardItem();
        typeItem->setData(type, Qt::EditRole);

        QStandardItem *valueItem = new QStandardItem();

        if (type == "File")
        {
            valueItem->setData(fileName, Qt::EditRole);
            valueItem->setData(filePath, Qt::UserRole);
        }
        else
        {
            valueItem->setData(textValue, Qt::EditRole);
        }

        QStandardItem *descriptionItem = new QStandardItem();
        descriptionItem->setData(description, Qt::EditRole);

        reqFormBodyModel.insertRow(reqFormBodyModel.rowCount(), { keyItem, typeItem, valueItem, descriptionItem });
    }
}

void RestyCageWindow::on_reqHeadersTableView_doubleClicked(const QModelIndex &index)
{
    editSimpleRow(reqHeadersModel, index.row(), index.column());
}

void RestyCageWindow::on_pushButton_2_clicked()
{
    removeModelRow(ui->reqBodyFormTableView, reqFormBodyModel);
}

void RestyCageWindow::on_reqAddUrlEncodedBodyRowBtn_clicked()
{
    addSimpleModelRow(reqUrlEncodedFormBodyModel);
}

void RestyCageWindow::on_reqRemoveUrlEncodedBodyRowBtn_clicked()
{
    removeModelRow(ui->reqUrlEncodedBodyTableView, reqUrlEncodedFormBodyModel);
}

void RestyCageWindow::on_reqUrlEncodedBodyTableView_doubleClicked(const QModelIndex &index)
{
    editSimpleRow(reqUrlEncodedFormBodyModel, index.row(), index.column());
}

