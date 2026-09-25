#include "ui_query_form.h"
#include "query_form.h"

#include "../constants.h"
#include "../db/query.h"

#include "../dialogs/collection_dialog.h"
#include "../db/db.h"
#include "../ui/json_highlighter.h"

#include <QFile>
#include <QFileDialog>
#include <QHttpPart>
#include <QHttpMultiPart>
#include <QJsonDocument>
#include <QMimeDatabase>
#include <QSaveFile>
#include <QPixmap>

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
    ui->rawContentTypeComboBox->setVisible(false);
    ui->exportBtn->setVisible(false);

    m_settings = new QSettings(settingsOrgKey, settingsAppKey, this);

    pdfView = new QPdfView(this);
    pdfDocument = new QPdfDocument(this);

    pdfView->setPageMode(QPdfView::PageMode::MultiPage);
    pdfView->setPageSpacing(10);
    pdfView->setDocument(pdfDocument);

    ui->pdfBodyPage->layout()->addWidget(pdfView);

    initModels();
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
    m_uuid = query.uid();
    m_queryId = query.id();
    m_name = query.name();
    m_collectionId = query.collectionId();

    int idx = ui->methodComboBox->findText(query.method());
    ui->methodComboBox->setCurrentIndex(idx);

    ui->authComboBox->setCurrentIndex(query.authType());

    loadItemsFromCollection(m_reqParamsModel, query.parameters());
    loadItemsFromCollection(m_reqHeadersModel, query.headers());

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

    loadItemsFromCollection(m_reqFormBodyModel, query.multipartFormBody(), true);
    loadItemsFromCollection(m_reqUrlEncodedFormBodyModel, query.encodedFormBody());

    if (query.bodyType() == BodyType::Raw && query.rawBody().has_value())
    {
        m_rawBodyId = query.rawBody()->id();

        ui->rawContentTypeComboBox->setCurrentIndex(query.rawBody()->rawBodyType());
        if (query.rawBody().value().rawBodyType() == QueryRawBody::RawBodyType::JSON)
        {
            new JsonHighlighter(ui->reqRawBodyTextEdit->document());
        }

        ui->reqRawBodyTextEdit->setText(query.rawBody()->value());
    }

    if (query.bodyType() == BodyType::Binary && query.binaryBody().has_value())
    {
        m_binaryBodyFilePath = query.binaryBody()->filepath();
        m_binaryBodyId = query.binaryBody()->id();

        QFileInfo fileInfo(m_binaryBodyFilePath);
        ui->reqFileSelectionLbl->setText(fileInfo.fileName());
    }
}

void QueryForm::initFromVariantMap(QVariantMap &queryVariant)
{
    const QString name = queryVariant.take(serializationName).toString();

    emit changedName(this, name);

    const QString method = queryVariant.take(serializationMethod).toString();
    const QString url = queryVariant.take(serializationUrl).toString();

    int idx = ui->methodComboBox->findText(method);
    ui->methodComboBox->setCurrentIndex(idx);
    ui->urlEdit->setText(url);

    if (queryVariant.contains(serializationCollectionId))
    {
        m_collectionId = queryVariant.take(serializationCollectionId).toInt();
    }

    if (queryVariant.contains(serializationBasicAuthId))
    {
        m_basicAuthId = queryVariant.take(serializationBasicAuthId).toInt();
    }

    if (queryVariant.contains(serializationBinaryBodyId))
    {
        m_binaryBodyId = queryVariant.take(serializationBinaryBodyId).toInt();
    }

    if (queryVariant.contains(serializationRawBodyId))
    {
        m_rawBodyId = queryVariant.take(serializationRawBodyId).toInt();
    }

    if (queryVariant.contains(serializationParams))
    {
        QVariantList params = queryVariant.take(serializationParams).toList();
        QList<ParamValue> paramValues = convertVariantListToParamValueList(params);

        loadItemsFromCollection(m_reqParamsModel, paramValues);
    }

    if (queryVariant.contains(serializationAuthentication))
    {
        QVariantMap authentication = queryVariant.take(serializationAuthentication).toMap();

        if (authentication.contains(serializationUsername))
        {
            const QString username = authentication.take(serializationUsername).toString();
            const QString password = authentication.take(serializationPassword).toString();

            ui->authBasicUserEdit->setText(username);
            ui->authBasicPasswordEdit->setText(password);
            ui->authComboBox->setCurrentIndex(AuthType::Basic);
        }

        if (authentication.contains(serializationBearerToken))
        {
            const QString bearerToken = authentication.take(serializationBearerToken).toString();

            ui->bearerTokenEdit->setText(bearerToken);
            ui->authComboBox->setCurrentIndex(AuthType::BearerToken);
        }
    }

    if (queryVariant.contains(serializationHeaders))
    {
        QVariantList headers = queryVariant.take(serializationHeaders).toList();
        QList<ParamValue> paramValues = convertVariantListToParamValueList(headers);

        loadItemsFromCollection(m_reqHeadersModel, paramValues);
    }

    if (queryVariant.contains(serializationBodyType) && queryVariant.contains(serializationBody))
    {
        const QString bodyType = queryVariant.take(serializationBodyType).toString();
        const QVariant body = queryVariant.take(serializationBody).toList();

        if (bodyType == bodyTypeFormData || bodyType == bodyTypeEncodedForm)
        {
            QList<ParamValue> paramValues = convertVariantListToParamValueList(body.toList());
            loadItemsFromCollection(m_reqFormBodyModel, paramValues);
        }

        if (bodyType == bodyTypeBinary)
        {
            m_binaryBodyFilePath = body.toString();

            QFileInfo fileInfo(m_binaryBodyFilePath);
            ui->reqFileSelectionLbl->setText(fileInfo.fileName());
        }

        if (bodyType == bodyTypeRaw)
        {
            QueryRawBody::RawBodyType rawBodyType;

            ui->rawContentTypeComboBox->setCurrentIndex(rawBodyType);
            if (rawBodyType == QueryRawBody::RawBodyType::JSON)
            {
                new JsonHighlighter(ui->reqRawBodyTextEdit->document());
            }

            ui->reqRawBodyTextEdit->setText(body.toString());
        }
    }

    if (queryVariant.contains(serializationReplyBody) && queryVariant.contains(serializationReplyType))
    {
        m_replyBody = queryVariant.take(serializationReplyBody).toByteArray();
        m_replyType = queryVariant.take(serializationReplyType).toString();

        loadReplyBody(m_replyBody, m_replyType);
    }

    if (queryVariant.contains(serializationReplyHeaders))
    {
        QVariantList replyHeaders = queryVariant.take(serializationReplyHeaders).toList();
        QMap<QString, QString> replyHeadersMap;

        for (QVariant &headerVariant: replyHeaders)
        {
            QVariantMap headerMap = headerVariant.toMap();
            replyHeadersMap.insert(headerMap.take(paramKey).toString(), headerMap.take(paramValue).toString());
        }

        loadReplyHeaders(replyHeadersMap);
    }
}

QUuid QueryForm::uid()
{
    return m_uuid;
}

void QueryForm::setEnvVariables(QMap<QString, QString> *envVariables)
{
    this->m_envVariables = envVariables;
}

QVariant QueryForm::serializeToVariant()
{
    const QString method = ui->methodComboBox->currentText();
    const QString bodyType = ui->reqBodyTypeComboBox->currentText();
    const QString url = ui->urlEdit->text();
    const QString authenticationType = ui->authComboBox->currentText();

    QVariantMap result;

    result.insert(serializationUuid, m_uuid);

    if (m_queryId.has_value())
    {
        result.insert(serializationId, m_queryId.value());
    }

    if (m_collectionId.has_value())
    {
        result.insert(serializationCollectionId, m_collectionId.value());
    }

    if (m_basicAuthId.has_value())
    {
        result.insert(serializationBasicAuthId, m_basicAuthId.value());
    }

    if (m_binaryBodyId.has_value())
    {
        result.insert(serializationBinaryBodyId, m_binaryBodyId.value());
    }

    if (m_rawBodyId)
    {
        result.insert(serializationRawBodyId, m_rawBodyId.value());
    }

    result.insert(serializationName, QVariant(m_name));
    result.insert(serializationUrl, QVariant(url));
    result.insert(serializationMethod, QVariant(method));

    if (authenticationType != authTypeNone)
    {
        QVariantMap authentication;

        if (authenticationType == authTypeBasicAuth)
        {
            const QString username = ui->authBasicUserEdit->text();
            const QString password = ui->authBasicPasswordEdit->text();

            authentication.insert(serializationUsername, username);
            authentication.insert(serializationPassword, password);
        }

        if (authenticationType == authTypeBearerToken)
        {
            const QString bearerToken = ui->bearerTokenEdit->text();
            authentication.insert(serializationBearerToken, bearerToken);
        }

        result.insert(serializationAuthentication, authentication);
    }

    if (m_reqParamsModel.rowCount() > 0)
    {
        QVariantList params = convertModelToVariantList(m_reqParamsModel);
        result.insert(serializationParams, params);
    }

    if (m_reqHeadersModel.rowCount() > 0)
    {
        QVariantList headers = convertModelToVariantList(m_reqHeadersModel);
        result.insert(serializationHeaders, headers);
    }

    if (bodyType != bodyTypeNone)
    {
        result.insert(serializationBodyType, bodyType);

        if (bodyType == bodyTypeFormData && m_reqFormBodyModel.rowCount() > 0)
        {
            QVariantList body = convertModelToVariantList(m_reqFormBodyModel, true);
            result.insert(serializationBody, body);
        }

        if (bodyType == bodyTypeEncodedForm && m_reqUrlEncodedFormBodyModel.rowCount())
        {
            QVariantList body = convertModelToVariantList(m_reqUrlEncodedFormBodyModel);
            result.insert(serializationBody, body);
        }

        if (bodyType == bodyTypeRaw && !ui->reqRawBodyTextEdit->document()->isEmpty())
        {
            result.insert(serializationBody, QVariant(ui->reqRawBodyTextEdit->toPlainText()));
        }

        if (bodyType == bodyTypeBinary)
        {
            result.insert(serializationBody, QVariant(m_binaryBodyFilePath));
        }
    }

    if (m_replyBody.has_value())
    {
        result.insert(serializationReplyBody, QVariant(m_replyBody.value()));
    }

    if (ui->respHeadersTableWidget->rowCount() > 0)
    {
        QVariantList replyHeaders;
        for (int i = 0; i < ui->respHeadersTableWidget->rowCount(); i++)
        {
            const QString key = ui->respHeadersTableWidget->item(i, 0)->text();
            const QString value = ui->respHeadersTableWidget->item(i, 1)->text();

            QVariantMap replyHeader;
            replyHeader.insert(paramKey, key);
            replyHeader.insert(paramValue, value);

            replyHeaders << replyHeader;
        }

        result.insert(serializationReplyHeaders, replyHeaders);
    }

    return result;
}

void QueryForm::initModels()
{
    m_reqParamsModel.insertColumns(0, 2);
    m_reqParamsModel.setHeaderData(0, Qt::Horizontal, QObject::tr(nameHeader));
    m_reqParamsModel.setHeaderData(1, Qt::Horizontal, QObject::tr(valueHeader));

    ui->reqParamsTableView->setModel(&m_reqParamsModel);

    m_reqHeadersModel.insertColumns(0, 2);
    m_reqHeadersModel.setHeaderData(0, Qt::Horizontal, QObject::tr(nameHeader));
    m_reqHeadersModel.setHeaderData(1, Qt::Horizontal, QObject::tr(valueHeader));

    ui->reqHeadersTableView->setModel(&m_reqHeadersModel);

    m_reqFormBodyModel.insertColumns(0, 3);
    m_reqFormBodyModel.setHeaderData(0, Qt::Horizontal, QObject::tr(nameHeader));
    m_reqFormBodyModel.setHeaderData(1, Qt::Horizontal, QObject::tr(typeHeader));
    m_reqFormBodyModel.setHeaderData(2, Qt::Horizontal, QObject::tr(valueHeader));

    ui->reqBodyFormTableView->setModel(&m_reqFormBodyModel);

    m_reqUrlEncodedFormBodyModel.insertColumns(0, 2);
    m_reqUrlEncodedFormBodyModel.setHeaderData(0, Qt::Horizontal, QObject::tr(nameHeader));
    m_reqUrlEncodedFormBodyModel.setHeaderData(1, Qt::Horizontal, QObject::tr(valueHeader));

    ui->reqUrlEncodedBodyTableView->setModel(&m_reqUrlEncodedFormBodyModel);
}

void QueryForm::on_sendButton_clicked()
{
    if (ui->sendButton->text() == "Send")
    {
        ui->requestTabWidget->setDisabled(true);
        ui->responseTabWidget->setDisabled(true);
        ui->sendButton->setText("Cancel");

        QString urlString = replaceEnvParameters(ui->urlEdit->text());
        QUrl url(urlString);

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

        map.insert(requestName, key);
        map.insert(requestValue, value);

        ParamValue param(map);
        paramVal << param;
    }

    m_networkHelper->setRequestsUrlParams(url, paramVal);
}

void QueryForm::setRequestAuth()
{
    if (ui->authComboBox->currentText() == authTypeBearerToken)
    {
        QString bearerToken = ui->bearerTokenEdit->text();
        QString bearerTokenParamReplace = replaceEnvParameters(bearerToken);

        m_networkHelper->setRequestBearerAuth(bearerTokenParamReplace);
    }

    if (ui->authComboBox->currentText() == authTypeBasicAuth)
    {
        QString user = replaceEnvParameters(ui->authBasicUserEdit->text());
        QString password = replaceEnvParameters(ui->authBasicPasswordEdit->text());

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

        headerMap.insert(requestName, replaceEnvParameters(headerKey));
        headerMap.insert(requestValue, replaceEnvParameters(headerValue));

        ParamValue param(headerMap);

        headers.append(param);
    }

    m_networkHelper->setRequestHeaders(headers);
}

void QueryForm::sendRequest(QUrlQuery &urlQuery)
{
    const QString method = ui->methodComboBox->currentText();

    const QString bodyType = ui->reqBodyTypeComboBox->currentText();

    if (bodyType == bodyTypeFormData)
    {
        sendMultiPartRequest(method);
    }
    else if (bodyType == bodyTypeEncodedForm)
    {
        sendUrlEncodedFormRequest(method, urlQuery);
    }
    else if (bodyType == bodyTypeRaw)
    {
        sendRawRequest(method);
    }
    else if (bodyType == bodyTypeBinary)
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
        int id = -1;
        QVariant idVariant = m_reqFormBodyModel.item(i, 0)->data(Qt::UserRole);

        if (idVariant.isNull())
        {
            id = idVariant.toInt();
        }

        QString key = m_reqFormBodyModel.item(i, 0)->data(Qt::EditRole).toString();
        QString type = m_reqFormBodyModel.item(i, 1)->data(Qt::EditRole).toString();
        QString value = m_reqFormBodyModel.item(i, 2)->data(Qt::EditRole).toString();
        QString pathValue = m_reqFormBodyModel.item(i, 2)->data(Qt::UserRole).toString();

        QMap<QString, QString> paramMap;
        paramMap.insert(requestName, key);
        paramMap.insert(requestValue, type != "File" ? replaceEnvParameters(value) : pathValue);

        ParamValue param(paramMap);

        if (id != -1)
        {
            param.setId(id);
        }

        param.setValueType(type == "File" ? ParamValue::ParamValueType::File : ParamValue::ParamValueType::String);
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
        paramMap.insert(requestName, key);
        paramMap.insert(requestValue, replaceEnvParameters(value));

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
            if (i == 0 && j == 0)
            {
                QVariant idData = itemsModel.item(i, j)->data(Qt::UserRole);
                if (!idData.isNull() && idData.toInt() > 0)
                {
                    id = idData.toInt();
                }
            }

            QVariant itemData = itemsModel.headerData(j, Qt::Orientation::Horizontal, Qt::DisplayRole);
            QVariant displayVariant = itemsModel.item(i, j)->data(Qt::DisplayRole);

            QVariant userVariant = itemsModel.item(i, j)->data(Qt::UserRole);

            paramValueMap.insert(
                itemData.toString().toLower(),
                userVariant.isNull() ? displayVariant.toString() : userVariant.toString());
        }

        ParamValue param(paramValueMap);
        if (paramValueMap.contains(paramType) && paramValueMap[paramType] == paramTypeFile)
        {
            param.setValueType(ParamValue::ParamValueType::File);
        }

        if (id > -1)
        {
            param.setId(id);
        }

        parameters.append(param);
    }

    return parameters;
}

QVariantList QueryForm::convertModelToVariantList(const QStandardItemModel &itemsModel, bool hasType)
{
    QVariantList parameters;

    for (int i = 0; i < itemsModel.rowCount(); i++)
    {
        QVariantMap paramMap;

        const QVariant id = itemsModel.item(i, 0)->data(Qt::UserRole);
        const QString key = itemsModel.item(i, 0)->data(Qt::EditRole).toString();
        const QString value = itemsModel.item(i,  hasType ? 2 : 1)->data(Qt::EditRole).toString();
        const QString description = m_reqParamsModel.item(i, hasType ? 3 : 2)->data(Qt::EditRole).toString();

        QVariantMap param;

        if (!id.isNull() && id.isValid())
        {
            param.insert(paramId, id.toInt());
        }

        param.insert(paramKey, key);
        if (hasType)
        {
            const QString type = itemsModel.item(i, 1)->data(Qt::UserRole).toString();
            const QVariant pathValue = itemsModel.item(i, hasType ? 2 : 1)->data(Qt::UserRole);

            param.insert(paramType, type);

            if (!pathValue.isNull())
            {
                param.insert(paramFilePathValue, pathValue.toString());
            }
        }

        param.insert(paramValue, value);
        param.insert(paramDescription, description);

        parameters << param;
    }

    return parameters;
}

QList<ParamValue> QueryForm::convertVariantListToParamValueList(const QVariantList &variantList)
{
    QList<ParamValue> paramValues;

    for (const QVariant &paramVariant : variantList.toList())
    {
        QVariantMap paramMap = paramVariant.toMap();
        QMap<QString, QString> paramValueMap;
        ParamValue::ParamValueType paramType = ParamValue::ParamValueType::String;

        paramValueMap.insert(paramKey, paramMap.take(paramKey).toString());

        if (paramValueMap.contains(paramFilePathValue))
        {
            paramValueMap.insert(paramValue, paramMap.take(paramFilePathValue).toString());
            paramType = ParamValue::ParamValueType::File;
        }
        else
        {
            paramValueMap.insert(paramValue, paramMap.take(paramValue).toString());
        }

        paramValueMap.insert(paramDescription, paramMap.take(paramDescription).toString());

        paramValues << ParamValue(paramValueMap, paramType);
    }

    return paramValues;
}

void QueryForm::loadItemsFromCollection(QStandardItemModel &itemsModel, QList<ParamValue> &vals, bool addType)
{
    for (ParamValue &paramVal: vals)
    {
        QList<QStandardItem*> rowItems;

        QStandardItem *nameItem = new QStandardItem();
        nameItem->setText(paramVal.value("name"));
        nameItem->setData(paramVal.id().value(), Qt::UserRole);

        rowItems.append(nameItem);

        if (paramVal.getValueType() == ParamValue::ParamValueType::File)
        {
            if (addType)
            {
                rowItems.append(new QStandardItem(paramTypeFile));
            }

            QString rawValue = paramVal.value(paramValue);
            QFileInfo fileInfo(rawValue);

            QStandardItem *fileItem = new QStandardItem(fileInfo.fileName());
            fileItem->setData(rawValue, Qt::UserRole);
            rowItems.append(fileItem);
        }
        else
        {
            if (addType)
            {
                rowItems.append(new QStandardItem(paramTypeText));
            }
            rowItems.append(new QStandardItem(paramVal.value(paramValue)));
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

    query.setUid(m_uuid);

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

    AuthType authType = Query::authTypeFromString(ui->authComboBox->currentText());
    query.setAuthType(authType);

    if (authType == AuthType::Basic)
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
    else if (authType == AuthType::BearerToken)
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

    BodyType bodyType = Query::bodyTypeFromString(ui->reqBodyTypeComboBox->currentText());
    query.setBodyType(bodyType);

    switch (bodyType) {
    case BodyType::Raw:
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
    case BodyType::EncodedForm:
        query.setEncodedFormBody(convertModelToParamValueList(m_reqUrlEncodedFormBodyModel, 3));
        break;
    case BodyType::MultipartForm:
        query.setMultipartFormBody(convertModelToParamValueList(m_reqFormBodyModel, 3));
        break;
    case BodyType::Binary:
    {
        QueryBinaryBody binaryBody(m_binaryBodyFilePath);
        if (m_queryId.has_value())
        {
            binaryBody.setQueryId(m_queryId.value());
        }

        if (m_binaryBodyId.has_value())
        {
            binaryBody.setId(m_binaryBodyId.value());
        }

        query.setBinaryBody(binaryBody);
    }
        break;
    default:
        break;
    }

    query.setDeletedParameters(m_deletedParams);
    query.setDeletedHeaders(m_deletedHeaders);
    query.setDeletedMultipartParams(m_deletedMultiPartParams);
    query.setDeletedEncodedFormParams(m_deletedEncodedFormParams);

    return query;
}

QString QueryForm::replaceEnvParameters(const QString &originalString)
{
    QRegularExpression regExp("{{(?<param>\\w+)}}");

    QRegularExpressionMatchIterator it = regExp.globalMatch(originalString);

    QString result = originalString;

    while (it.hasNext())
    {
        QRegularExpressionMatch match = it.next();
        QString param = match.captured("param");

        if (param.isNull() || !m_envVariables->contains(param))
        {
            continue;
        }

        result = result.replace(QString("{{%0}}").arg(param), m_envVariables->value(param));
    }

    return result;
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
        ui->exportBtn->setVisible(true);
    }
    else
    {
        ui->statusLbl->setText("");
        ui->sizeLbl->setText("");
        ui->timeLbl->setText("");

        ui->respBodyTextEdit->setPlainText("");
        ui->respHeadersTableWidget->clear();
        ui->respHeadersTableWidget->setRowCount(0);
    }

    ui->requestTabWidget->setDisabled(false);
    ui->responseTabWidget->setDisabled(false);
    ui->sendButton->setText("Send");
}

void QueryForm::loadReplyBody(std::optional<QByteArray> replyBody, std::optional<QString> replyType)
{
    m_replyBody = replyBody.has_value() ? replyBody : m_networkHelper->replyBody();
    m_replyType = replyType.has_value() ? replyType : m_networkHelper->replyType();

    if (!m_replyType.has_value())
    {
        return;
    }

    if (m_replyType.value().contains("image"))
    {
        QPixmap pixmap;
        if (pixmap.loadFromData(m_replyBody.value()))
        {
            ui->imgLabel->setPixmap(pixmap);
            ui->respBodyStackedWidget->setCurrentWidget(ui->imgBodyPage);
            return;
        }
    }

    if (m_replyType.value().contains("pdf"))
    {
        ui->respBodyStackedWidget->setCurrentWidget(ui->pdfBodyPage);
        pdfDocument->close();

        QBuffer buff(&m_replyBody.value(), nullptr);
        buff.open(QIODevice::ReadOnly);
        pdfDocument->load(&buff);
        buff.close();

        return;
    }

    ui->respBodyStackedWidget->setCurrentWidget(ui->textBodyPage);

    if (m_replyType.value().contains("application/json"))
    {
        QJsonDocument jsonDocument = QJsonDocument::fromJson(m_replyBody.value());        
        ui->respBodyTextEdit->setPlainText(jsonDocument.toJson(QJsonDocument::Indented));

        ui->respBodyTextEdit->setUpdatesEnabled(false);
        ui->respBodyTextEdit->document()->setUndoRedoEnabled(false);

        new JsonHighlighter(ui->respBodyTextEdit->document());

        ui->respBodyTextEdit->setUpdatesEnabled(true);
        ui->respBodyTextEdit->document()->setUndoRedoEnabled(true);
    }
    else
    {
        ui->respBodyTextEdit->setPlainText(m_replyBody.value());
    }
}

void QueryForm::loadReplyHeaders(std::optional<QMap<QString, QString>> replyHeadersMap)
{
    QMap<QString, QString> replyHeaders = replyHeadersMap.has_value()
        ? replyHeadersMap.value()
        : m_networkHelper->replyHeaders();

    QMapIterator<QString, QString> it(replyHeaders);

    ui->respHeadersTableWidget->clear();
    ui->respHeadersTableWidget->setRowCount(0);

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
    keyValueHandler->addSimpleRowModel(this, itemsModel);
}

void QueryForm::editSimpleRow(QStandardItemModel &itemsModel, int row, int column)
{
    keyValueHandler->editSimpleRowModel(this, itemsModel, row, column);
}

QList<QVariant> QueryForm::removeModelRow(QTableView *tableView, QStandardItemModel &itemsModel)
{
    return keyValueHandler->deleteSimpleRowModel(tableView, itemsModel);
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
    keyValueHandler->addRowModel(this, m_reqFormBodyModel);
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

void QueryForm::on_rawContentTypeComboBox_currentIndexChanged(int index)
{
    if (index == 0)
    {
        new JsonHighlighter(ui->reqRawBodyTextEdit->document());
    }
}


void QueryForm::on_exportBtn_clicked()
{
    QByteArray& data = m_networkHelper->replyBody();
    QMimeDatabase db;
    QString replyType = m_networkHelper->replyType().split(";").first().trimmed();

    QMimeType mimeType = db.mimeTypeForName(replyType);

    QString folderWritePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString proposedFilePath = QString("%1/%2.%3").arg(folderWritePath, "response", mimeType.preferredSuffix());
    QString fileFilter = mimeType.filterString();

    QString filePath = QFileDialog::getSaveFileName(this, "Save Response", proposedFilePath, fileFilter);

    if (filePath.isEmpty())
    {
        return;
    }

    QSaveFile file(filePath);

    if (file.open(QIODevice::WriteOnly))
    {
        file.write(data);
        file.commit();
    }
}

void QueryForm::on_reqBodyFormTableView_doubleClicked(const QModelIndex &index)
{
    //editSimpleRow(m_reqFormBodyModel, index.row(), index.column());
    keyValueHandler->editRowModel(this, m_reqFormBodyModel, index.row(), index.column());
}

