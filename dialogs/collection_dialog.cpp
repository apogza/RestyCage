#include "collection_dialog.h"
#include "ui_collection_dialog.h"

CollectionDialog::CollectionDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CollectionDialog)
{
    ui->setupUi(this);
}

CollectionDialog::CollectionDialog(QList<Collection> &collectionList, QWidget *parent)
    : CollectionDialog(parent)
{
    setCollectionList(collectionList);
}

CollectionDialog::~CollectionDialog()
{
    delete ui;
}

void CollectionDialog::setCollectionList(QList<Collection> &collectionList)
{
    ui->parentComboBox->clear();
    ui->parentComboBox->addItem("");

    for (Collection &collection : collectionList)
    {
        ui->parentComboBox->addItem(collection.name(), collection.id().value());
    }
}

QString CollectionDialog::name()
{
    return ui->nameEdit->text();
}

std::optional<int> CollectionDialog::collectionId()
{
    QVariant data = ui->parentComboBox->currentData();

    if (data.isNull())
    {
        return std::nullopt;
    }
    else
    {
        return data.toInt();
    }
}
