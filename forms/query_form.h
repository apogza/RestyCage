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
    QNetworkAccessManager *m_nam;
    QStandardItemModel m_reqParamsModel;
    QStandardItemModel m_reqHeadersModel;
    QStandardItemModel m_reqUrlEncodedFormBodyModel;
    QStandardItemModel m_reqFormBodyModel;
    QString m_binaryBodyFilePath;

    QString m_name;
    std::optional<int> m_queryId;
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
    void setRequestAuth(QNetworkRequest &req);
    void setRequestHeaders(QNetworkRequest &req);

    void sendRequest(QNetworkRequest &request, QUrlQuery &urlQuery);
    QNetworkReply *sendMultiPartRequest(QNetworkRequest &request, const QString &method);
    QNetworkReply *sendUrlEncodedFormRequest(QNetworkRequest &request, const QString &method, QUrlQuery &urlQuery);
    QNetworkReply *sendRawRequest(QNetworkRequest &request, const QString &method);
    QNetworkReply *sendBinaryRequest(QNetworkRequest &request, const QString &method);

    QList<ParamValue> convertModelToParamValueList(const QStandardItemModel &itemsModel, int numColumns);
    Query createQuery();


    void readReply();
    void readReplyHeaders(QNetworkReply* reply);
};



#endif // QUERY_FORM_H
