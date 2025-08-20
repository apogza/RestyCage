#ifndef ENVIRONMENTFORM_H
#define ENVIRONMENTFORM_H

#include "../keyvaluehandler.h"

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

    void initFromFile(QString &fileName);

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
    QStandardItemModel envItemModel;
    KeyValueHandler *keyValueHandler;
    QString fileName;

    void initModel();
};

#endif // ENVIRONMENTFORM_H
