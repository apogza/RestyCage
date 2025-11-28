#ifndef QUERY_FORM_H
#define QUERY_FORM_H

#include "../key_value_handler.h"
#include "../db/param_value.h"
#include "../db/query.h"
#include "../db/db.h"

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QStandardItemModel>
#include <QTableView>
#include <QNetworkReply>
#include <QUrlQuery>
#include "../network_helper.h"


namespace Ui {
class QueryForm;
}

class QueryForm : public QWidget
{
    Q_OBJECT

public:
    explicit QueryForm(QWidget *parent = nullptr);
    ~QueryForm();

    void initFromDb(Query &query);
    QUuid uid();

signals:
    void changedName(QueryForm *form, QString newName);
    void hasBeenModified(QueryForm *form);

private slots:
    void on_sendButton_clicked();
    void on_authComboBox_currentIndexChanged(int index);
    void on_reqAddParamBtn_clicked();
    void on_reqRemoveParamBtn_clicked();
    void on_reqHeadersAddBtn_clicked();
    void on_reqHeadersRemoveBtn_clicked();
    void on_reqBodyTypeComboBox_currentIndexChanged(int index);

    void on_reqParamsTableView_doubleClicked(const QModelIndex &index);

    void on_reqBodyFormDataAddRowBtn_clicked();

    void on_reqBodyFormDataRemoveRowBtn_clicked();

    void on_reqHeadersTableView_doubleClicked(const QModelIndex &index);

    void on_reqAddUrlEncodedBodyRowBtn_clicked();

    void on_reqRemoveUrlEncodedBodyRowBtn_clicked();

    void on_reqUrlEncodedBodyTableView_doubleClicked(const QModelIndex &index);

    void on_reqFileSelectionBtn_clicked();

    void on_saveQueryBtn_clicked();

private:
    Ui::QueryForm *ui;
    NetworkHelper *m_networkHelper;
    QStandardItemModel m_reqParamsModel;
    QStandardItemModel m_reqHeadersModel;
    QStandardItemModel m_reqUrlEncodedFormBodyModel;
    QStandardItemModel m_reqFormBodyModel;
    QString m_binaryBodyFilePath;

    const int tabStop = 4;

    QString m_name;
    QUuid m_uid = QUuid::createUuid();
    std::optional<int> m_queryId;
    std::optional<int> m_collectionId;
    std::optional<int> m_rawBodyId;
    std::optional<int> m_binaryBodyId;
    std::optional<int> m_basicAuthId;
    std::optional<int> m_bearerAuthId;

    Db &m_db = Db::instance();

    QList<int> m_deletedParams;
    QList<int> m_deletedHeaders;
    QList<int> m_deletedMultiPartParams;
    QList<int> m_deletedEncodedFormParams;

    quint64 requestStartMs;
    KeyValueHandler *keyValueHandler;

    void initModels();
    void addSimpleModelRow(QStandardItemModel &itemsModel);
    void editSimpleRow(QStandardItemModel &itemsModel, int row, int column);
    QList<QVariant> removeModelRow(QTableView* tableView, QStandardItemModel &itemsModel);

    void setRequestParams(QUrlQuery &url);
    void setRequestAuth();
    void setRequestHeaders();

    void sendRequest(QUrlQuery &urlQuery);
    void sendMultiPartRequest(const QString &method);
    void sendUrlEncodedFormRequest(const QString &method, QUrlQuery &urlQuery);
    void sendRawRequest(const QString &method);
    void sendBinaryRequest(const QString &method);

    QList<ParamValue> convertModelToParamValueList(const QStandardItemModel &itemsModel, int numColumns);
    void loadItemsFromDb(QStandardItemModel &itemsModel, QList<ParamValue> &vals);
    Query createQuery();

    void slotReplyReceived();
    void loadReplyBody();
    void loadReplyHeaders();

    void saveQuery();

};



#endif // QUERY_FORM_H
