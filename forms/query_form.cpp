#include "../constants.h"
#include "ui_query_form.h"
#include "../dialogs/key_value_file_text_dialog.h"
#include "../db/query.h"
#include "query_form.h"
#include "../dialogs/collection_dialog.h"
#include "../db/db.h"

#include <QFile>
#include <QFileDialog>
#include <QHttpPart>
#include <QHttpMultiPart>
#include <QJsonDocument>
#include "../ui/json_highlighter.h"

QueryForm::QueryForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QueryForm)
{
    ui->setupUi(this);

    m_networkHelper = new NetworkHelper(this);
    connect(m_networkHelper, &NetworkHelper::replyReceived, this, &QueryForm::slotReplyReceived);

    keyValueHandler = new KeyValueHandler(this);

    QFont font = ui->respBodyTextEdit->font();
    QFontMetrics fontMetrics(font);
    QSize tabSize = fontMetrics.size(Qt::TextSingleLine, " ");

    ui->reqRawBodyTextEdit->setTabStopDistance(tabStop * tabSize.width());

    ui->respHeadersTableWidget->setColumnCount(2);
    ui->respHeadersTableWidget->setHorizontalHeaderLabels(QStringList() << nameHeader << valueHeader);
    ui->respBodyTextEdit->setAutoFormatting(QTextEdit::AutoAll);    
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
    //indicate to the main window what the name of query is so that the tab title is set
    emit changedName(this, query.name());

    ui->urlEdit->setText(query.url());
    m_uid = query.uid();
    m_queryId = query.id();
    m_name = query.name();
    m_collectionId = query.collectionId();

    if (query.rawBody().has_value())
    {
        m_rawBodyId = query.rawBody()->id();
    }

    if (query.binaryBody().has_value())
    {
        m_binaryBodyId = query.binaryBody()->id();
    }

    int idx = ui->methodComboBox->findText(query.method());
    ui->methodComboBox->setCurrentIndex(idx);

    ui->authComboBox->setCurrentIndex(query.authType());


    loadItemsFromDb(m_reqParamsModel, query.parameters());
    loadItemsFromDb(m_reqHeadersModel, query.headers());

    if (query.basicAuth().has_value())
    {
        m_basicAuthId = query.basicAuth().value().id();
        ui->authBasicUserEdit->setText(query.basicAuth().value().username());
        ui->authBasicPasswordEdit->setText(query.basicAuth().value().password());
    }

    if (query.bearerAuth().has_value())
    {
        m_bearerAuthId = query.bearerAuth().value().id();
        ui->bearerTokenEdit->setText(query.bearerAuth().value().bearerToken());
    }

    ui->reqBodyTypeComboBox->setCurrentIndex(query.bodyType());

    loadItemsFromDb(m_reqFormBodyModel, query.multipartFormBody());
    loadItemsFromDb(m_reqUrlEncodedFormBodyModel, query.encodedFormBody());

    if (query.bodyType() == Query::BodyType::Raw && query.rawBody().has_value())
    {
        ui->rawContentTypeComboBox->setCurrentIndex(query.rawBody()->rawBodyType());
        ui->reqRawBodyTextEdit->setText(query.rawBody()->value());
    }
}

QUuid QueryForm::uid()
{
    return m_uid;
}

void QueryForm::initModels()
{
    m_reqParamsModel.insertColumns(0, 3);
    m_reqParamsModel.setHeaderData(0, Qt::Horizontal, QObject::tr(nameHeader));
    m_reqParamsModel.setHeaderData(1, Qt::Horizontal, QObject::tr(valueHeader));
    m_reqParamsModel.setHeaderData(2, Qt::Horizontal, QObject::tr(descriptionHeader));

    ui->reqParamsTableView->setModel(&m_reqParamsModel);

    m_reqHeadersModel.insertColumns(0, 3);
    m_reqHeadersModel.setHeaderData(0, Qt::Horizontal, QObject::tr(nameHeader));
    m_reqHeadersModel.setHeaderData(1, Qt::Horizontal, QObject::tr(valueHeader));
    m_reqHeadersModel.setHeaderData(2, Qt::Horizontal, QObject::tr(descriptionHeader));

    ui->reqHeadersTableView->setModel(&m_reqHeadersModel);

    m_reqFormBodyModel.insertColumns(0, 4);
    m_reqFormBodyModel.setHeaderData(0, Qt::Horizontal, QObject::tr(nameHeader));
    m_reqFormBodyModel.setHeaderData(1, Qt::Horizontal, QObject::tr(typeHeader));
    m_reqFormBodyModel.setHeaderData(2, Qt::Horizontal, QObject::tr(valueHeader));
    m_reqFormBodyModel.setHeaderData(3, Qt::Horizontal, QObject::tr(descriptionHeader));

    ui->reqBodyFormTableView->setModel(&m_reqFormBodyModel);

    m_reqUrlEncodedFormBodyModel.insertColumns(0, 3);
    m_reqUrlEncodedFormBodyModel.setHeaderData(0, Qt::Horizontal, QObject::tr(nameHeader));
    m_reqUrlEncodedFormBodyModel.setHeaderData(1, Qt::Horizontal, QObject::tr(valueHeader));
    m_reqUrlEncodedFormBodyModel.setHeaderData(2, Qt::Horizontal, QObject::tr(descriptionHeader));

    ui->reqUrlEncodedBodyTableView->setModel(&m_reqUrlEncodedFormBodyModel);
}

void QueryForm::on_sendButton_clicked()
{

    if (ui->sendButton->text() == "Send")
    {
        ui->requestTabWidget->setDisabled(true);
        ui->responseTabWidget->setDisabled(true);
        ui->sendButton->setText("Cancel");

        QUrl url(ui->urlEdit->text());

        QUrlQuery urlQuery(url);
        setRequestParams(urlQuery);

        m_networkHelper->initRequest(url);

        setRequestAuth();
        setRequestHeaders();

        sendRequest(urlQuery);
    }
    else
    {
        m_networkHelper->cancelRequest();
    }
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
        int id = -1;

        for (int j = 0; j < numColumns; j++)
        {
            QVariant headerData = itemsModel.headerData(j, Qt::Orientation::Horizontal, Qt::DisplayRole);
            QVariant itemData = itemsModel.item(i, j)->data(Qt::UserRole);
            if (!itemData.isNull())
            {
                id = itemData.toInt();
            }

            paramValueMap.insert(
                headerData.toString().toLower(),
                itemsModel.item(i, j)->data(Qt::DisplayRole).toString());
        }

        ParamValue param(paramValueMap);

        if (id > -1)
        {
            param.setId(id);
        }

        parameters.append(param);
    }

    return parameters;
}

void QueryForm::loadItemsFromDb(QStandardItemModel &itemsModel, QList<ParamValue> &vals)
{

    for (ParamValue &paramVal: vals)
    {
        QList<QStandardItem*> rowItems;

        QStandardItem *nameItem = new QStandardItem();
        nameItem->setText(paramVal.value("name"));
        nameItem->setData(paramVal.id().value(), Qt::UserRole);

        rowItems.append(nameItem);
        rowItems.append(new QStandardItem(paramVal.value("value")));
        if (paramVal.hasValue("description"))
        {
            rowItems.append(new QStandardItem(paramVal.value("description")));
        }

        itemsModel.insertRow(itemsModel.rowCount(), rowItems);
    }
}


Query QueryForm::createQuery()
{
    Query query;
    query.setName(m_name);

    if (m_queryId.has_value())
    {
        query.setId(m_queryId.value());
    }

    if (m_collectionId.has_value())
    {
        query.setCollectionId(m_collectionId.value());
    }

    query.setUid(m_uid);

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

    Query::AuthType authType = Query::authTypeFromString(ui->authComboBox->currentText());
    query.setAuthType(authType);

    if (authType == Query::AuthType::Basic)
    {
        QString username = ui->authBasicUserEdit->text();
        QString password = ui->authBasicPasswordEdit->text();

        BasicQueryAuth basicAuth(username, password);

        if (m_queryId.has_value())
        {
            basicAuth.setQueryId(m_queryId.value());
        }

        if (m_basicAuthId.has_value())
        {
            basicAuth.setId(m_basicAuthId.value());
        }

        query.setBasicAuth(basicAuth);
    }
    else if (authType == Query::AuthType::BearerToken)
    {
        QString bearerToken = ui->bearerTokenEdit->text();
        BearerQueryAuth bearerAuth(bearerToken);

        if (m_bearerAuthId.has_value())
        {
            bearerAuth.setId(m_bearerAuthId.value());
        }

        if (m_queryId.has_value())
        {
            bearerAuth.setQueryId(m_queryId.value());
        }

        query.setBearerAuth(bearerAuth);
    }

    Query::BodyType bodyType = Query::bodyTypeFromString(ui->reqBodyTypeComboBox->currentText());
    query.setBodyType(bodyType);

    switch (bodyType) {
    case Query::BodyType::Raw:
    {
        QueryRawBody::RawBodyType rawBodyType = QueryRawBody::rawBodyTypeFromString(ui->rawContentTypeComboBox->currentText());
        QString value = ui->reqRawBodyTextEdit->toPlainText();

        if (m_rawBodyId.has_value() && m_queryId.has_value())
        {
            QueryRawBody rawBody(m_rawBodyId.value(), m_queryId.value(), rawBodyType, value);
            query.setRawBody(rawBody);
        }
        else
        {
            QueryRawBody rawBody(rawBodyType, value);
            query.setRawBody(rawBody);
        }
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

    query.setDeletedParameters(m_deletedParams);
    query.setDeletedHeaders(m_deletedHeaders);
    query.setDeletedMultipartParams(m_deletedMultiPartParams);
    query.setDeletedEncodedFormParams(m_deletedEncodedFormParams);

    return query;
}

void QueryForm::slotReplyReceived()
{
    if (m_networkHelper->statusCode() != 0)
    {
        ui->statusLbl->setText(QString("HTTP %1").arg(QString::number(m_networkHelper->statusCode())));
        ui->sizeLbl->setText(QString("%1 bytes").arg(QString::number(m_networkHelper->replyBody().size())));
        ui->timeLbl->setText(QString("%1 ms").arg(QString::number(m_networkHelper->replyTotalTime())));

        loadReplyBody();
        loadReplyHeaders();
    }
    else
    {
        ui->statusLbl->setText("");
        ui->sizeLbl->setText("");
        ui->timeLbl->setText("");

        ui->respBodyTextEdit->setText("");
        ui->respHeadersTableWidget->clearContents();
    }

    ui->requestTabWidget->setDisabled(false);
    ui->responseTabWidget->setDisabled(false);
    ui->sendButton->setText("Send");
}

void QueryForm::loadReplyBody()
{
    QByteArray replyBody = m_networkHelper->replyBody();
    QString replyType = m_networkHelper->replyType();


    if (replyType.contains("application/json"))
    {
        QJsonDocument jsonDocument = QJsonDocument::fromJson(m_networkHelper->replyBody());
        ui->respBodyTextEdit->setText(jsonDocument.toJson(QJsonDocument::Indented));
        JsonHighlighter *highlighter = new JsonHighlighter(ui->respBodyTextEdit->document());
    }
    else
    {
        ui->respBodyTextEdit->setText(m_networkHelper->replyBody());
    }
}

void QueryForm::loadReplyHeaders()
{
    QMap<QString, QString> replyHeaders = m_networkHelper->replyHeaders();
    QMapIterator<QString, QString> it(replyHeaders);

    while (it.hasNext())
    {
        it.next();

        QTableWidgetItem *itemKey = new QTableWidgetItem();
        itemKey->setText(it.key());

        QTableWidgetItem *itemValue = new QTableWidgetItem();
        itemValue->setText(it.value());

        int rowNum = ui->respHeadersTableWidget->rowCount();

        ui->respHeadersTableWidget->insertRow(rowNum);
        ui->respHeadersTableWidget->setItem(rowNum, 0, itemKey);
        ui->respHeadersTableWidget->setItem(rowNum, 1, itemValue);
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
    saveQuery();
}

void QueryForm::saveQuery()
{
    Query query = createQuery();

    bool isNewQuery = !m_queryId.has_value();

    if (isNewQuery)
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
        query.setName(queryName);
        query.setCollectionId(collectionId.value());

        m_name = queryName;
        m_collectionId = collectionId.value();
    }

    bool saveResult = m_db.saveQuery(query);

    if (saveResult)
    {
        m_deletedParams.clear();
        m_deletedHeaders.clear();
        m_deletedMultiPartParams.clear();
        m_deletedEncodedFormParams.clear();

        m_queryId = query.id();
    }

    emit changedName(this, query.name());
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
