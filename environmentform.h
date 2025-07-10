#ifndef ENVIRONMENTFORM_H
#define ENVIRONMENTFORM_H

#include <QWidget>
#include <QStandardItemModel>
#include "keyvaluehandler.h"

namespace Ui {
class EnvironmentForm;
}

class EnvironmentForm : public QWidget
{
    Q_OBJECT

public:
    explicit EnvironmentForm(QWidget *parent = nullptr);
    ~EnvironmentForm();

private slots:
    void on_addEnvironmentBtn_clicked();

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_removeEnvironmentBtn_clicked();

private:
    Ui::EnvironmentForm *ui;
    QStandardItemModel envItemModel;
    KeyValueHandler *keyValueHandler;

    void initModel();
};

#endif // ENVIRONMENTFORM_H
