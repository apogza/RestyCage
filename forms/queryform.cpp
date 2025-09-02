#include "../constants.h"
#include "ui_queryform.h"
#include "../dialogs/keyvaluefiletextdialog.h"
#include "../qjsonmodel.h"
#include "../db/query.h"
#include "queryform.h"
#include "../queryserializer.h"
#include "../dialogs/namedialog.h"
#include "../db/db.h"

#include <QFile>
#include <QFileDialog>
#include <QHttpPart>
#include <QHttpMultiPart>


QueryForm::QueryForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QueryForm)
{
    ui->setupUi(this);

    m_nam = new QNetworkAccessManager(this);
    keyValueHandler = new KeyValueHandler(this);

    ui->respHeadersTableWidget->setColumnCount(2);
    ui->respHeadersTableWidget->setHorizontalHeaderLabels(QStringList() << keyHeader << valueHeader);
    ui->rawContentTypeComboBox->setVisible(false);

    initModels();
    Db &db = Db::instance();
}

QueryForm::~QueryForm()
{
    delete ui;
}

void QueryForm::initModels()
{
    m_reqParamsModel.insertColumns(0, 3);
    m_reqParamsModel.setHeaderData(0, Qt::Horizontal, QObject::tr(keyHeader));
    m_reqParamsModel.setHeaderData(1, Qt::Horizontal, QObject::tr(valueHeader));
    m_reqParamsModel.setHeaderData(2, Qt::Horizontal, QObject::tr(descriptionHeader));

    ui->reqParamsTableView->setModel(&m_reqParamsModel);

    m_reqHeadersModel.insertColumns(0, 3);
    m_reqHeadersModel.setHeaderData(0, Qt::Horizontal, QObject::tr(keyHeader));
    m_reqHeadersModel.setHeaderData(1, Qt::Horizontal, QObject::tr(valueHeader));
    m_reqHeadersModel.setHeaderData(2, Qt::Horizontal, QObject::tr(descriptionHeader));

    ui->reqHeadersTableView->setModel(&m_reqHeadersModel);

    m_reqFormBodyModel.insertColumns(0, 4);
    m_reqFormBodyModel.setHeaderData(0, Qt::Horizontal, QObject::tr(keyHeader));
    m_reqFormBodyModel.setHeaderData(1, Qt::Horizontal, QObject::tr(typeHeader));
    m_reqFormBodyModel.setHeaderData(2, Qt::Horizontal, QObject::tr(valueHeader));
    m_reqFormBodyModel.setHeaderData(3, Qt::Horizontal, QObject::tr(descriptionHeader));

    ui->reqBodyFormTableView->setModel(&m_reqFormBodyModel);

    m_reqUrlEncodedFormBodyModel.insertColumns(0, 3);
    m_reqUrlEncodedFormBodyModel.setHeaderData(0, Qt::Horizontal, QObject::tr(keyHeader));
    m_reqUrlEncodedFormBodyModel.setHeaderData(1, Qt::Horizontal, QObject::tr(valueHeader));
    m_reqUrlEncodedFormBodyModel.setHeaderData(2, Qt::Horizontal, QObject::tr(descriptionHeader));

    ui->reqUrlEncodedBodyTableView->setModel(&m_reqUrlEncodedFormBodyModel);
}

void QueryForm::on_sendButton_clicked()
{
    ui->requestTabWidget->setDisabled(true);
    ui->responseTabWidget->setDisabled(true);

    QUrl url(ui->urlEdit->text());

    QUrlQuery urlQuery(url);
    setRequestParams(urlQuery);

    url.setQuery(urlQuery);
    QNetworkRequest req(url);

    setRequestAuth(req);
    setRequestHeaders(req);

    sendRequest(req, urlQuery);
}

void QueryForm::setRequestParams(QUrlQuery &url)
{
    for (int i = 0; i < m_reqParamsModel.rowCount(); i++)
    {
        QString key = m_reqParamsModel.item(i, 0)->text();
        QString value = m_reqParamsModel.item(i, 1)->text();
        url.addQueryItem(key, value);
    }
}

void QueryForm::setRequestAuth(QNetworkRequest &req)
{
    if (ui->authComboBox->currentText() == "Bearer Token")
    {
        QString bearerToken = ui->bearerTokenEdit->text();
        req.setRawHeader(authorizationHeader, QString("Bearer %1").arg(bearerToken).toUtf8());
    }

    if (ui->authComboBox->currentText() == "Basic Auth")
    {
        QString user = ui->authBasicUserEdit->text();
        QString password = ui->authBasicPasswordEdit->text();

        req.setRawHeader(authorizationHeader, QString("Basic %1:%2").arg(user, password).toUtf8());
    }
}

void QueryForm::setRequestHeaders(QNetworkRequest &req)
{
    for (int i = 0; i < m_reqHeadersModel.rowCount(); i++)
    {
        QString headerKey = m_reqHeadersModel.item(i, 0)->text();
        QString headerValue = m_reqHeadersModel.item(i, 1)->text();

        req.headers().append(headerKey, headerValue);
    }
}

void QueryForm::sendRequest(QNetworkRequest &request, QUrlQuery &urlQuery)
{
    const QString method = ui->methodComboBox->currentText();

    QString bodyType = ui->reqBodyTypeComboBox->currentText();

    QNetworkReply *reply = nullptr;

    if (bodyType == "Form Data")
    {
        reply = sendMultiPartRequest(request, method);
    }
    else if (bodyType == "X-www-encoded-form")
    {
        reply = sendUrlEncodedFormRequest(request, method, urlQuery);
    }
    else if (bodyType == "Raw")
    {
        reply = sendRawRequest(request, method);
    }
    else if (bodyType == "Binary")
    {
        reply = sendBinaryRequest(request, method);
    }
    else
    {
        reply = m_nam->sendCustomRequest(request, method.toUtf8());
    }

    requestStartMs = QDateTime::currentMSecsSinceEpoch();

    connect(reply, &QNetworkReply::finished, this, &QueryForm::readReply);
}

QNetworkReply *QueryForm::sendMultiPartRequest(QNetworkRequest &request, const QString &method)
{
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("multipart/form-data"));
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    for (int i = 0; i < m_reqFormBodyModel.rowCount(); i++)
    {
        QString key = m_reqFormBodyModel.item(i, 0)->data(Qt::EditRole).toString();
        QString type = m_reqFormBodyModel.item(i, 1)->data(Qt::EditRole).toString();
        QString value = m_reqFormBodyModel.item(i, 2)->data(Qt::UserRole).toString();

        QHttpPart part;
        part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString("form-data; name=\"%1\"").arg(key)));

        if (type == "File")
        {
            part.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
            QFile *file = new QFile(value, multiPart);

            file->open(QIODevice::ReadOnly);
            part.setBodyDevice(file);
            multiPart->append(part);
        }
        else
        {
            part.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
            part.setBody(value.toUtf8());
        }
    }

    return m_nam->sendCustomRequest(request, method.toUtf8(), multiPart);
}

QNetworkReply *QueryForm::sendUrlEncodedFormRequest(QNetworkRequest &request, const QString &method, QUrlQuery &urlQuery)
{
    for (int i = 0; i < m_reqUrlEncodedFormBodyModel.rowCount(); i++)
    {
        QString key = m_reqUrlEncodedFormBodyModel.item(i, 0)->data(Qt::EditRole).toString();
        QString value = m_reqUrlEncodedFormBodyModel.item(i, 1)->data(Qt::EditRole).toString();

        urlQuery.addQueryItem(key, value);
    }

    return m_nam->sendCustomRequest(request, method.toUtf8());
}

QNetworkReply *QueryForm::sendRawRequest(QNetworkRequest &request, const QString &method)
{
    QString body = ui->reqRawBodyTextEdit->toPlainText();
    QString bodyType = ui->rawContentTypeComboBox->currentText();

    if (bodyType == "JSON")
    {
        request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    }
    else if (bodyType == "XML")
    {
        request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/xml"));
    }
    else if (bodyType == "HTML")
    {
        request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/html"));
    }
    else if (bodyType == "JavaScript")
    {
        request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/javascript"));
    }
    else
    {
        request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    }

    return m_nam->sendCustomRequest(request, method.toUtf8(), body.toUtf8());
}

QNetworkReply *QueryForm::sendBinaryRequest(QNetworkRequest &request, const QString &method)
{
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));

    QFile file(m_binaryBodyFilePath);
    file.open(QIODevice::ReadOnly);
    QByteArray fileByteArray = file.readAll();

    return m_nam->sendCustomRequest(request, method.toUtf8(), fileByteArray);
}

QList<ParamValue> QueryForm::convertModelToParamValueList(const QStandardItemModel &itemsModel, int numColumns)
{
    QList<ParamValue> parameters;
    for (int i = 0; i < itemsModel.rowCount(); i++)
    {
        QMap<QString, QString> paramValueMap;

        for (int j = 0; j < numColumns; j++)
        {
            QVariant headerData = itemsModel.headerData(j, Qt::Orientation::Horizontal, Qt::DisplayRole);

            paramValueMap.insert(
                headerData.toString(),
                itemsModel.item(i, j)->data(Qt::DisplayRole).toString());
        }

        parameters.append(ParamValue(paramValueMap));
    }

    return parameters;
}

Query QueryForm::createQuery()
{
    Query query;
    query.setName(m_name);
    query.setMethod(ui->methodComboBox->currentText());
    query.setUrl(ui->urlEdit->text());

    if (m_reqParamsModel.rowCount() > 0)
    {
        query.setParameters(convertModelToParamValueList(m_reqParamsModel, 3));
    }

    if (m_reqHeadersModel.rowCount() > 0)
    {
        query.setHeaders(convertModelToParamValueList(m_reqHeadersModel, 3));
    }

    Query::BodyType bodyType = Query::bodyTypeFromString(ui->reqBodyTypeComboBox->currentText());
    Query::AuthType authType = Query::authTypeFromString(ui->authComboBox->currentText());

    switch (bodyType) {
    case Query::BodyType::Raw:
        query.setRawBodyType(Query::rawBodyTypeFromString(ui->rawContentTypeComboBox->currentText()));
        query.setRawBodyValue(ui->reqRawBodyTextEdit->toPlainText());
        break;
    case Query::BodyType::EncodedForm:
        query.setEncodedForm(convertModelToParamValueList(m_reqUrlEncodedFormBodyModel, 3));
        break;
    case Query::BodyType::MultipartForm:
        query.setMultipartForm(convertModelToParamValueList(m_reqUrlEncodedFormBodyModel, 3));
        break;
    case Query::BodyType::Binary:
        query.setBinaryForm(m_binaryBodyFilePath);
        break;
    default:
        break;
    }

    if (authType == Query::AuthType::Basic)
    {
        query.setAuthType(Query::AuthType::Basic);

        BasicQueryAuth basicAuth(ui->authBasicUserEdit->text(), ui->authBasicPasswordEdit->text());
        query.setBasicAuth(basicAuth);
    }
    else if (authType == Query::AuthType::BearerToken)
    {
        query.setAuthType(Query::AuthType::BearerToken);

        QString bearerToken = ui->bearerTokenEdit->text();
        BearerQueryAuth bearerAuth(bearerToken);
        query.setBearerAuth(bearerAuth);
    }
    else
    {
        query.setAuthType(Query::AuthType::None);
    }

    return query;
}

void QueryForm::readReply()
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

    ui->requestTabWidget->setDisabled(false);
    ui->responseTabWidget->setDisabled(false);
}

void QueryForm::readReplyHeaders(QNetworkReply *reply)
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

void QueryForm::on_authComboBox_currentIndexChanged(int index)
{
    ui->authStackedWidget->setCurrentIndex(index);
}

void QueryForm::addSimpleModelRow(QStandardItemModel &itemsModel)
{
    keyValueHandler->addRowModel(this, itemsModel);
}

void QueryForm::editSimpleRow(QStandardItemModel &itemsModel, int row, int column)
{
    keyValueHandler->editRowModel(this, itemsModel, row, column);
}

void QueryForm::removeModelRow(QTableView *tableView, QStandardItemModel &itemsModel)
{
    keyValueHandler->deleteRowModel(tableView, itemsModel);
}

void QueryForm::on_reqAddParamBtn_clicked()
{
    addSimpleModelRow(m_reqParamsModel);
}

void QueryForm::on_reqRemoveParamBtn_clicked()
{
    removeModelRow(ui->reqParamsTableView, m_reqParamsModel);
}

void QueryForm::on_reqHeadersAddBtn_clicked()
{
    addSimpleModelRow(m_reqHeadersModel);
}

void QueryForm::on_reqHeadersRemoveBtn_clicked()
{
    removeModelRow(ui->reqHeadersTableView, m_reqHeadersModel);
}

void QueryForm::on_reqBodyTypeComboBox_currentIndexChanged(int index)
{
    ui->reqBodyStackedWidget->setCurrentIndex(index);
    ui->rawContentTypeComboBox->setVisible(ui->reqBodyTypeComboBox->currentText() == "Raw");
}

void QueryForm::on_reqParamsTableView_doubleClicked(const QModelIndex &index)
{
    editSimpleRow(m_reqParamsModel, index.row(), index.column());
}

void QueryForm::on_reqBodyFormDataAddRowBtn_clicked()
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

        m_reqFormBodyModel.insertRow(m_reqFormBodyModel.rowCount(), { keyItem, typeItem, valueItem, descriptionItem });
    }
}

void QueryForm::on_reqHeadersTableView_doubleClicked(const QModelIndex &index)
{
    editSimpleRow(m_reqHeadersModel, index.row(), index.column());
}

void QueryForm::on_pushButton_2_clicked()
{
    removeModelRow(ui->reqBodyFormTableView, m_reqFormBodyModel);
}

void QueryForm::on_reqAddUrlEncodedBodyRowBtn_clicked()
{
    addSimpleModelRow(m_reqUrlEncodedFormBodyModel);
}

void QueryForm::on_reqRemoveUrlEncodedBodyRowBtn_clicked()
{
    removeModelRow(ui->reqUrlEncodedBodyTableView, m_reqUrlEncodedFormBodyModel);
}

void QueryForm::on_reqUrlEncodedBodyTableView_doubleClicked(const QModelIndex &index)
{
    editSimpleRow(m_reqUrlEncodedFormBodyModel, index.row(), index.column());
}

void QueryForm::on_reqFileSelectionBtn_clicked()
{
    QFileDialog fileDialog;
    int result = fileDialog.exec();

    if (result == QDialog::Accepted)
    {
        m_binaryBodyFilePath = fileDialog.selectedFiles().at(0);
        QFileInfo fileInfo(m_binaryBodyFilePath);
        ui->reqFileSelectionLbl->setText(fileInfo.fileName());
    }
}

void QueryForm::on_saveQueryBtn_clicked()
{
    NameDialog nameDialog;

    Query query = createQuery();
    m_db.saveQuery(query);
}
