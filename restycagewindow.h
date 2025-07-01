#ifndef RESTYCAGEWINDOW_H
#define RESTYCAGEWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QStandardItemModel>
#include <QTableView>
#include <QNetworkReply>


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

    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::RestyCageWindow *ui;
    QNetworkAccessManager *nam;
    QStandardItemModel reqParamsModel;
    QStandardItemModel reqHeadersModel;

    void initModels();
    void addModelRow(QStandardItemModel &itemsModel);
    void removeModelRow(QTableView* tableView, QStandardItemModel &itemsModel);

    void setRequestParams(QUrlQuery &url);
    void setRequestAuth(QNetworkRequest &req);
    void setRequestBody(QNetworkRequest &req);
    void setRequestHeaders(QNetworkRequest &req);

    void sendRequest(QNetworkRequest &request);
    void readReply();
    void readReplyHeaders(QNetworkReply* reply);


};
#endif // RESTYCAGEWINDOW_H
