#include "environmentform.h"
#include "ui_environmentform.h"
#include "environmentserializer.h"

EnvironmentForm::EnvironmentForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EnvironmentForm)
{
    ui->setupUi(this);
    keyValueHandler = new KeyValueHandler(this);

    initModel();
}

EnvironmentForm::~EnvironmentForm()
{
    delete ui;
}

void EnvironmentForm::initModel()
{
    envItemModel.insertColumns(0, 3);
    envItemModel.setHeaderData(0, Qt::Horizontal, QObject::tr("Key"));
    envItemModel.setHeaderData(1, Qt::Horizontal, QObject::tr("Value"));
    envItemModel.setHeaderData(2, Qt::Horizontal, QObject::tr("Description"));

    ui->tableView->setModel(&envItemModel);
}

void EnvironmentForm::on_addEnvironmentBtn_clicked()
{
    keyValueHandler->addRowModel(this, envItemModel);
}

void EnvironmentForm::on_tableView_doubleClicked(const QModelIndex &index)
{
    keyValueHandler->editRowModel(this, envItemModel, index.row(), index.column());
}

void EnvironmentForm::on_removeEnvironmentBtn_clicked()
{
    keyValueHandler->deleteRowModel(ui->tableView, envItemModel);
}


void EnvironmentForm::on_saveEnvironmentBtn_clicked()
{
    EnvironmentSerializer serializer;

    for(int i = 0; i < envItemModel.rowCount(); i++)
    {
        QStandardItem *keyItem = envItemModel.item(i, 0);
        QStandardItem *valueItem = envItemModel.item(i, 1);
        QStandardItem *descriptionItem = envItemModel.item(i, 2);

        serializer.addVariable(keyItem->text(), valueItem->text(), descriptionItem->text());
    }

    serializer.saveToFile("Test.json");
}

