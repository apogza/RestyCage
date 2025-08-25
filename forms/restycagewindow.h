#ifndef RESTYCAGEWINDOW_H
#define RESTYCAGEWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QMap>
#include <QDir>

#include "../db/db.h"

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

public slots:
    void onTabHasChangedName(QWidget *widget, QString newName);
    void onTabHasBeenModified(QWidget *widget);

private slots:

    void on_tabWidget_tabCloseRequested(int index);

    void on_newQueryBtn_clicked();

    void on_addEnvironmentBtn_clicked();

    void on_envsTreeView_doubleClicked(const QModelIndex &index);

    void on_newCollectionBtn_clicked();

    void on_delEnvBtn_clicked();

    void on_removeCollectionBtn_clicked();

private:
    Ui::RestyCageWindow *ui;
    QStandardItemModel m_envsModel;
    QStandardItemModel m_collectionsModel;
    QMap<QString, QWidget*> m_tabs;
    Db &m_db = Db::instance();

    void addNewQuery();
    void initEnvironments();
    void initCollections();

    void initAppDataFolder();

};
#endif // RESTYCAGEWINDOW_H
