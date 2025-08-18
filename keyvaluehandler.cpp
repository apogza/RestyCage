#include "keyvaluehandler.h"
#include "keyvaluedialog.h"

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
    QStandardItem *keyItem = itemsModel.item(row, 0);
    QStandardItem *valueItem = itemsModel.item(row, 1);
    QStandardItem *descriptionItem = itemsModel.item(row, 2);

    QString key = keyItem->data(Qt::EditRole).toString();
    QString value = valueItem->data(Qt::EditRole).toString();
    QString description = descriptionItem->data(Qt::EditRole).toString();

    KeyValueDialog *keyValueDialog = new KeyValueDialog(widget, key, value, description);
    int result = keyValueDialog->exec();

    if (result == QDialog::Accepted)
    {
        QStandardItem *item = itemsModel.item(row, 0);
        item->setData(keyValueDialog->getKey(), Qt::EditRole);

        item = itemsModel.item(row, 1);
        item->setData(keyValueDialog->getValue(), Qt::EditRole);

        item = itemsModel.item(row, 2);
        item->setData(keyValueDialog->getDescription(), Qt::EditRole);
        return true;
    }

    return false;
}

void KeyValueHandler::deleteRowModel(QTableView *tableView, QStandardItemModel &itemsModel)
{
    QItemSelectionModel *selectionModel = tableView->selectionModel();
    QModelIndexList indexlist = selectionModel->selectedIndexes();

    foreach (QModelIndex idx, indexlist)
    {
        itemsModel.removeRow(idx.row());
    }
}
