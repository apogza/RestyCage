#ifndef ENVIRONMENT_FORM_H
#define ENVIRONMENT_FORM_H

#include "../key_value_handler.h"
#include "../db/db.h"

#include <QWidget>
#include <QStandardItemModel>

namespace Ui {
class EnvironmentForm;
}

class EnvironmentForm : public QWidget
{
    Q_OBJECT

public:
    explicit EnvironmentForm(QWidget *parent = nullptr);
    ~EnvironmentForm();

    void initFromDb(Environment &env);

    QUuid uid();

signals:
    void changedName(EnvironmentForm *form, QString newName);
    void hasBeenModified(EnvironmentForm *form);

private slots:
    void on_addEnvironmentBtn_clicked();

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_removeEnvironmentBtn_clicked();

    void on_saveEnvironmentBtn_clicked();

private:
    Ui::EnvironmentForm *ui;
    QStandardItemModel m_envItemModel;
    KeyValueHandler *m_keyValueHandler;
    QString m_envName;
    QUuid m_uid = QUuid::createUuid();
    std::optional<int> m_envId;
    Db &m_db = Db::instance();

    QList<int> m_deletedParams;


    void initModel();
};

#endif // ENVIRONMENT_FORM_H
