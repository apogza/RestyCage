#include "../constants.h"
#include "ui_query_form.h"
#include "../dialogs/key_value_file_text_dialog.h"
#include "../qjsonmodel.h"
#include "../db/query.h"
#include "query_form.h"
#include "../dialogs/collection_dialog.h"
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

    m_networkHelper = new NetworkHelper(this);
    connect(m_networkHelper, &NetworkHelper::replyReceived, this, &QueryForm::slotReplyReceived);

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

void QueryForm::initFromDb(Query &query)
{
    emit changedName(this, query.name());
    ui->urlEdit->setText(query.url());
    m_queryId = query.id();

    int idx = ui->methodComboBox->findText(query.method());
    ui->methodComboBox->setCurrentIndex(idx);
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

    m_networkHelper->initRequest(url);

    setRequestAuth();
    setRequestHeaders();

    sendRequest(urlQuery);
}

void QueryForm::setRequestParams(QUrlQuery &url)
{
    QList<ParamValue> paramVal;

    for (int i = 0; i < m_reqParamsModel.rowCount(); i++)
    {
        QMap<QString, QString> map;

        QString key = m_reqParamsModel.item(i, 0)->text();
        QString value = m_reqParamsModel.item(i, 1)->text();

        map.insert("name", key);
        map.insert("value", value);

        ParamValue param(map);
        paramVal << param;
    }

    m_networkHelper->setRequestsUrlParams(url, paramVal);
}

void QueryForm::setRequestAuth()
{
    if (ui->authComboBox->currentText() == "Bearer Token")
    {
        QString bearerToken = ui->bearerTokenEdit->text();
        m_networkHelper->setRequestBearerAuth(bearerToken);
    }

    if (ui->authComboBox->currentText() == "Basic Auth")
    {
        QString user = ui->authBasicUserEdit->text();
        QString password = ui->authBasicPasswordEdit->text();

        m_networkHelper->setRequestBasicAuth(user, password);
    }
}

void QueryForm::setRequestHeaders()
{
    QList<ParamValue> headers;

    for (int i = 0; i < m_reqHeadersModel.rowCount(); i++)
    {
        QMap<QString, QString> headerMap;

        QString headerKey = m_reqHeadersModel.item(i, 0)->text();
        QString headerValue = m_reqHeadersModel.item(i, 1)->text();

        headerMap.insert("name", headerKey);
        headerMap.insert("value", headerValue);

        ParamValue param(headerMap);

        headers.append(param);
    }

    m_networkHelper->setRequestHeaders(headers);
}

void QueryForm::sendRequest(QUrlQuery &urlQuery)
{
    const QString method = ui->methodComboBox->currentText();

    QString bodyType = ui->reqBodyTypeComboBox->currentText();

    if (bodyType == "Form Data")
    {
        sendMultiPartRequest(method);
    }
    else if (bodyType == "X-www-encoded-form")
    {
        sendUrlEncodedFormRequest(method, urlQuery);
    }
    else if (bodyType == "Raw")
    {
        sendRawRequest(method);
    }
    else if (bodyType == "Binary")
    {
        sendBinaryRequest(method);
    }
    else
    {
        m_networkHelper->sendCustomRequest(method.toUtf8());
    }
}

void QueryForm::sendMultiPartRequest(const QString &method)
{
    QList<ParamValue> params;

    for (int i = 0; i < m_reqFormBodyModel.rowCount(); i++)
    {
        QString key = m_reqFormBodyModel.item(i, 0)->data(Qt::EditRole).toString();
        QString type = m_reqFormBodyModel.item(i, 1)->data(Qt::EditRole).toString();
        QString value = m_reqFormBodyModel.item(i, 2)->data(Qt::UserRole).toString();

        QMap<QString, QString> paramMap;
        paramMap.insert("name", key);
        paramMap.insert("type", type);
        paramMap.insert("value", value);

        ParamValue param(paramMap);
        params.append(param);
    }

    m_networkHelper->sendMultiPartRequest(method.toUtf8(), params);
}

void QueryForm::sendUrlEncodedFormRequest(const QString &method, QUrlQuery &urlQuery)
{
    QList<ParamValue> params;
    for (int i = 0; i < m_reqUrlEncodedFormBodyModel.rowCount(); i++)
    {
        QString key = m_reqUrlEncodedFormBodyModel.item(i, 0)->data(Qt::EditRole).toString();
        QString value = m_reqUrlEncodedFormBodyModel.item(i, 1)->data(Qt::EditRole).toString();

        urlQuery.addQueryItem(key, value);

        QMap<QString, QString> paramMap;
        paramMap.insert("name", key);
        paramMap.insert("value", value);

        ParamValue paramValue(paramMap);
        params.append(paramValue);
    }

    m_networkHelper->sendUrlEncodedFormRequest(method, urlQuery, params);
}

void QueryForm::sendRawRequest(const QString &method)
{
    QString body = ui->reqRawBodyTextEdit->toPlainText();
    QString bodyType = ui->rawContentTypeComboBox->currentText();

    m_networkHelper->sendRawRequest(method, bodyType, body.toUtf8());
}

void QueryForm::sendBinaryRequest(const QString &method)
{    
    m_networkHelper->sendBinaryRequest(method, m_binaryBodyFilePath);
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

    if (m_queryId.has_value())
    {
        query.setId(m_queryId.value());
    }

    QString method = ui->methodComboBox->currentText();
    QString url = ui->urlEdit->text() ;
    query.setMethod(method);
    query.setUrl(url);

    if (m_reqParamsModel.rowCount() > 0)
    {
        query.setParameters(convertModelToParamValueList(m_reqParamsModel, 3));
    }

    if (m_reqHeadersModel.rowCount() > 0)
    {
        query.setHeaders(convertModelToParamValueList(m_reqHeadersModel, 3));
    }

    Query::BodyType bodyType = Query::bodyTypeFromString(ui->reqBodyTypeComboBox->currentText());
    query.setBodyType(bodyType);

    switch (bodyType) {
    case Query::BodyType::Raw:
    {
        QueryRawBody::RawBodyType rawBodyType = QueryRawBody::rawBodyTypeFromString(ui->rawContentTypeComboBox->currentText());
        QString value = ui->reqRawBodyTextEdit->toPlainText();
        QueryRawBody rawBody(rawBodyType, value);

        query.setRawBody(rawBody);
        break;
    }
    case Query::BodyType::EncodedForm:
        query.setEncodedFormBody(convertModelToParamValueList(m_reqUrlEncodedFormBodyModel, 3));
        break;
    case Query::BodyType::MultipartForm:
        query.setMultipartFormBody(convertModelToParamValueList(m_reqUrlEncodedFormBodyModel, 3));
        break;
    case Query::BodyType::Binary:
    {
        QueryBinaryBody binaryBody(m_binaryBodyFilePath);
        query.setBinaryBody(binaryBody);
        break;
    }
    default:
        break;
    }

    Query::AuthType authType = Query::authTypeFromString(ui->authComboBox->currentText());
    query.setAuthType(authType);

    if (authType == Query::AuthType::Basic)
    {
        QString username = ui->authBasicUserEdit->text();
        QString password = ui->authBasicPasswordEdit->text();

        BasicQueryAuth basicAuth(username, password);
        query.setBasicAuth(basicAuth);
    }
    else if (authType == Query::AuthType::BearerToken)
    {
        QString bearerToken = ui->bearerTokenEdit->text();
        BearerQueryAuth bearerAuth(bearerToken);
        query.setBearerAuth(bearerAuth);
    }

    query.setDeletedParameters(m_deletedParams);
    query.setDeletedHeaders(m_deletedHeaders);
    query.setDeletedMultipartParams(m_deletedMultiPartParams);
    query.setDeletedEncodedFormParams(m_deletedEncodedFormParams);

    return query;
}

void QueryForm::slotReplyReceived()
{
    ui->statusLbl->setText(QString("HTTP %1").arg(QString::number(m_networkHelper->statusCode())));
    ui->sizeLbl->setText(QString("%1 bytes").arg(QString::number(m_networkHelper->replyBody().size())));
    ui->timeLbl->setText(QString("%1 ms").arg(QString::number(m_networkHelper->replyTotalTime())));

    QJsonModel *model = new QJsonModel(ui->respJsonTreeView);
    ui->respJsonTreeView->setModel(model);
    model->loadJson(m_networkHelper->replyBody());

    ui->requestTabWidget->setDisabled(false);
    ui->responseTabWidget->setDisabled(false);
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

QList<QVariant> QueryForm::removeModelRow(QTableView *tableView, QStandardItemModel &itemsModel)
{
    return keyValueHandler->deleteRowModel(tableView, itemsModel);
}

void QueryForm::on_reqAddParamBtn_clicked()
{
    addSimpleModelRow(m_reqParamsModel);
}

void QueryForm::on_reqRemoveParamBtn_clicked()
{
    QList<QVariant> removedItems = removeModelRow(ui->reqParamsTableView, m_reqParamsModel);

    for (QVariant &idVal : removedItems)
    {
        if (!idVal.isNull() && idVal.isValid())
        {
            m_deletedParams.append(idVal.toInt());
        }
    }
}

void QueryForm::on_reqHeadersAddBtn_clicked()
{
    addSimpleModelRow(m_reqHeadersModel);
}

void QueryForm::on_reqHeadersRemoveBtn_clicked()
{
    QList<QVariant> removedItems = removeModelRow(ui->reqHeadersTableView, m_reqHeadersModel);

    for (QVariant &idVal: removedItems)
    {
        if (!idVal.isNull() && idVal.isValid())
        {
            m_deletedHeaders.append(idVal.toInt());
        }
    }
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

void QueryForm::on_reqAddUrlEncodedBodyRowBtn_clicked()
{
    addSimpleModelRow(m_reqUrlEncodedFormBodyModel);
}

void QueryForm::on_reqRemoveUrlEncodedBodyRowBtn_clicked()
{
    QList<QVariant> removedItems = removeModelRow(ui->reqUrlEncodedBodyTableView, m_reqUrlEncodedFormBodyModel);

    for (QVariant &idVal : removedItems)
    {
        if (!idVal.isNull() && idVal.isValid())
        {
            m_deletedMultiPartParams.append(idVal.toInt());
        }
    }
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
    QList<Collection> collections = m_db.getCollections();

    CollectionDialog collectionDialog;
    collectionDialog.setCollectionList(collections);

    int dialogResult = collectionDialog.exec();

    if (dialogResult != QDialog::Accepted)
    {
        return;
    }

    std::optional<int> collectionId = collectionDialog.collectionId();

    if (!collectionId.has_value())
    {
        return;
    }

    QString queryName = collectionDialog.name();

    Query query = createQuery();
    query.setName(queryName);
    query.setCollectionId(collectionId.value());
    bool saveResult = m_db.saveQuery(query);

    if (saveResult)
    {
        m_deletedParams.clear();
        m_deletedHeaders.clear();
        m_deletedMultiPartParams.clear();
        m_deletedEncodedFormParams.clear();
    }
}

void QueryForm::on_reqBodyFormDataRemoveRowBtn_clicked()
{
    QList<QVariant> removedItems = removeModelRow(ui->reqBodyFormTableView, m_reqFormBodyModel);

    for (QVariant &idVal : removedItems)
    {
        if (!idVal.isNull() && idVal.isValid())
        {
            m_deletedMultiPartParams.append(idVal.toInt());
        }
    }
}
