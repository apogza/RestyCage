#ifndef RESTYCAGEWINDOW_H
#define RESTYCAGEWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkRequest>


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
    void on_comboBox_currentIndexChanged(int index);

    void on_authComboBox_currentIndexChanged(int index);

private:
    Ui::RestyCageWindow *ui;
    QNetworkAccessManager *nam;

    void setRequestParams(QNetworkRequest &req);
    void setRequestAuth(QNetworkRequest &req);
    void setRequestBody(QNetworkRequest &req);

    void sendRequest(QNetworkRequest &request);
    void readReply();



};
#endif // RESTYCAGEWINDOW_H
