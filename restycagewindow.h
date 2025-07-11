#ifndef RESTYCAGEWINDOW_H
#define RESTYCAGEWINDOW_H

#include <QMainWindow>

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

    void on_queryTabWidget_tabCloseRequested(int index);

    void on_newQueryBtn_clicked();

    void on_newCollectionBtn_clicked();

    void on_addEnvironmentBtn_clicked();

private:
    Ui::RestyCageWindow *ui;
    void addNewQuery();

};
#endif // RESTYCAGEWINDOW_H
