#include "keyvaluehandler.h"
#include "dialogs/keyvaluedialog.h"

KeyValueHandler::KeyValueHandler(QObject *parent)
    : QObject{parent}
{}

bool KeyValueHandler::addRowModel(QWidget *widget, QStandardItemModel &itemsModel)
{
    KeyValueDialog *dialog = new KeyValueDialog(widget);

    int result = dialog->exec();

    if (result == QDialog::Accepted)
    {
        QString key = dialog->getKey();
        QString value = dialog->getValue();
        QString description = dialog->getDescription();

        itemsModel.insertRow(itemsModel.rowCount(),
                            {
                                new QStandardItem(key),
                                new QStandardItem(value),
                                new QStandardItem(description)
                            }
        );
        return true;
    }

    return false;
}

bool KeyValueHandler::editRowModel(QWidget *widget, QStandardItemModel &itemsModel, int row, int column)
{
    QStandardItem *nameItem = itemsModel.item(row, 0);
    QStandardItem *valueItem = itemsModel.item(row, 1);
    QStandardItem *descriptionItem = itemsModel.item(row, 2);

    QVariant paramId = nameItem->data(Qt::UserRole);

    QString name = nameItem->data(Qt::EditRole).toString();
    QString value = valueItem->data(Qt::EditRole).toString();
    QString description = descriptionItem->data(Qt::EditRole).toString();

    KeyValueDialog *keyValueDialog = new KeyValueDialog(widget, name, value, description);
    int result = keyValueDialog->exec();

    if (result == QDialog::Accepted)
    {
        QStandardItem *item = itemsModel.item(row, 0);
        item->setData(keyValueDialog->getKey(), Qt::EditRole);
        item->setData(paramId, Qt::UserRole);

        item = itemsModel.item(row, 1);
        item->setData(keyValueDialog->getValue(), Qt::EditRole);

        item = itemsModel.item(row, 2);
        item->setData(keyValueDialog->getDescription(), Qt::EditRole);
        return true;
    }

    return false;
}

QList<QVariant> KeyValueHandler::deleteRowModel(QTableView *tableView, QStandardItemModel &itemsModel)
{
    QItemSelectionModel *selectionModel = tableView->selectionModel();
    QModelIndexList indexlist = selectionModel->selectedIndexes();

    QList<QVariant> userData;
    for (QModelIndex &idx : indexlist)
    {
        QVariant data = itemsModel.item(idx.row(), 0)->data(Qt::UserRole);
        if (!data.isNull())
        {
            userData.append(data);
        }

        itemsModel.removeRow(idx.row());        
    }

    return userData;
}
