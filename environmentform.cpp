#include "environmentform.h"
#include "ui_environmentform.h"
#include "environmentserializer.h"
#include "constants.h"

#include "namedialog.h"

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

void EnvironmentForm::initFromFile(QString &fileName)
{
    this->fileName = QString(fileName);

    EnvironmentSerializer serializer;
    serializer.loadModelFromFile(fileName, envItemModel);
}

void EnvironmentForm::initModel()
{
    envItemModel.insertColumns(0, 3);
    envItemModel.setHeaderData(0, Qt::Horizontal, QObject::tr(keyHeader));
    envItemModel.setHeaderData(1, Qt::Horizontal, QObject::tr(valueHeader));
    envItemModel.setHeaderData(2, Qt::Horizontal, QObject::tr(descriptionHeader));

    ui->tableView->setModel(&envItemModel);
}

void EnvironmentForm::on_addEnvironmentBtn_clicked()
{
    bool result = keyValueHandler->addRowModel(this, envItemModel);

    if (result)
    {
        emit hasBeenModified(this);
    }
}

void EnvironmentForm::on_tableView_doubleClicked(const QModelIndex &index)
{
    bool result = keyValueHandler->editRowModel(this, envItemModel, index.row(), index.column());

    if (result)
    {
        emit hasBeenModified(this);
    }
}

void EnvironmentForm::on_removeEnvironmentBtn_clicked()
{
    keyValueHandler->deleteRowModel(ui->tableView, envItemModel);
    emit hasBeenModified(this);
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

    if (fileName.isEmpty())
    {
        NameDialog nameDialog;
        int dialogResult = nameDialog.exec();

        if (dialogResult == QDialog::Accepted)
        {
            fileName = nameDialog.getName();
        }
    }

    serializer.saveToFile(fileName);
    emit changedName(this, fileName);
}

