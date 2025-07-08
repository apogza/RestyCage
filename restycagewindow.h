#ifndef RESTYCAGEWINDOW_H
#define RESTYCAGEWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QStandardItemModel>
#include <QTableView>
#include <QNetworkReply>
#include <QUrlQuery>

QT_BEGIN_NAMESPACE
namespace Ui {
class RestyCageWindow;
}
QT_END_NAMESPACE

class RestyCageWindow : public QMainWindow
{
    Q_OBJECT

public:
    RestyCageWindow(QWidget *parent = nullptr);
    ~RestyCageWindow();

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

    void on_reqHeadersTableView_doubleClicked(const QModelIndex &index);

    void on_pushButton_2_clicked();

    void on_reqAddUrlEncodedBodyRowBtn_clicked();

    void on_reqRemoveUrlEncodedBodyRowBtn_clicked();

    void on_reqUrlEncodedBodyTableView_doubleClicked(const QModelIndex &index);

    void on_reqFileSelectionBtn_clicked();

private:
    Ui::RestyCageWindow *ui;
    QNetworkAccessManager *nam;
    QStandardItemModel reqParamsModel;
    QStandardItemModel reqHeadersModel;
    QStandardItemModel reqUrlEncodedFormBodyModel;
    QStandardItemModel reqFormBodyModel;
    QString selectedBinaryBodyFilePath;
    quint64 requestStartMs;

    void initModels();
    void addSimpleModelRow(QStandardItemModel &itemsModel);
    void editSimpleRow(QStandardItemModel &itemsModel, int row, int column);
    void removeModelRow(QTableView* tableView, QStandardItemModel &itemsModel);

    void setRequestParams(QUrlQuery &url);
    void setRequestAuth(QNetworkRequest &req);
    void setRequestHeaders(QNetworkRequest &req);

    void sendRequest(QNetworkRequest &request, QUrlQuery &urlQuery);
    QNetworkReply *sendMultiPartRequest(QNetworkRequest &request, const QString &method);
    QNetworkReply *sendUrlEncodedFormRequest(QNetworkRequest &request, const QString &method, QUrlQuery &urlQuery);
    QNetworkReply *sendRawRequest(QNetworkRequest &request, const QString &method);
    QNetworkReply *sendBinaryRequest(QNetworkRequest &request, const QString &method);


    void readReply();
    void readReplyHeaders(QNetworkReply* reply);


};
#endif // RESTYCAGEWINDOW_H
