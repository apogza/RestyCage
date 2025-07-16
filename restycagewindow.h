#ifndef RESTYCAGEWINDOW_H
#define RESTYCAGEWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QMap>
#include <QDir>


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

    void on_addEnvironmentBtn_clicked();

    void on_envsTreeView_doubleClicked(const QModelIndex &index);

    void on_newCollectionBtn_clicked();

private:
    Ui::RestyCageWindow *ui;
    QStandardItemModel envsModel;
    QStandardItemModel collectionsModel;
    QMap<QString, QWidget*> tabs;


    void addNewQuery();
    void initEnvironments();
    void initCollections();
    void traverseCollectionsDir(QDir currentDir);

};
#endif // RESTYCAGEWINDOW_H
