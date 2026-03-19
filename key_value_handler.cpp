#include "key_value_handler.h"
#include "dialogs/key_value_dialog.h"
#include "dialogs/key_value_file_text_dialog.h"

KeyValueHandler::KeyValueHandler(QObject *parent)
    : QObject{parent}
{}

bool KeyValueHandler::addSimpleRowModel(QWidget *widget, QStandardItemModel &itemsModel)
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

bool KeyValueHandler::addRowModel(QWidget *widget, QStandardItemModel &itemsModel)
{
    KeyValueFileTextDialog *dialog = new KeyValueFileTextDialog(widget);

    int result = dialog->exec();
    if (result == QDialog::Accepted)
    {
        QString key = dialog->getKey();
        QString type = dialog->getType();
        QString filePath = dialog->getFilePathValue();
        QString fileName = dialog->getFileNameValue();
        QString textValue = dialog->getTextValue();

        QString description = dialog->getDescription();

        QStandardItem *keyItem = new QStandardItem();
        keyItem->setData(key, Qt::EditRole);

        QStandardItem *typeItem = new QStandardItem();
        typeItem->setData(type, Qt::EditRole);

        QStandardItem *valueItem = new QStandardItem();

        if (type == "File")
        {
            valueItem->setData(fileName, Qt::EditRole);
            valueItem->setData(filePath, Qt::UserRole);
        }
        else
        {
            valueItem->setData(textValue, Qt::EditRole);
        }

        QStandardItem *descriptionItem = new QStandardItem();
        descriptionItem->setData(description, Qt::EditRole);

        itemsModel.insertRow(itemsModel.rowCount(), { keyItem, typeItem, valueItem, descriptionItem });
        return true;
    }

    return false;
}

bool KeyValueHandler::editSimpleRowModel(QWidget *widget, QStandardItemModel &itemsModel, int row, int column)
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

bool KeyValueHandler::editRowModel(QWidget *widget, QStandardItemModel &itemsModel, int row, int column)
{
    QStandardItem *nameItem = itemsModel.item(row, 0);
    QStandardItem *typeItem = itemsModel.item(row, 1);
    QStandardItem *valueItem = itemsModel.item(row, 2);
    QStandardItem *descriptionItem = itemsModel.item(row, 3);

    QVariant paramId = nameItem->data(Qt::UserRole);

    QString name = nameItem->data(Qt::EditRole).toString();
    QString type = typeItem->data(Qt::EditRole).toString();
    QString value = type == "File" ? valueItem->data(Qt::UserRole).toString() : valueItem->data(Qt::EditRole).toString();
    QString description = descriptionItem->data(Qt::EditRole).toString();

    KeyValueFileTextDialog *keyValueDialog = new KeyValueFileTextDialog(widget, name, type, value, description);

    int result = keyValueDialog->exec();
    if (result == QDialog::Accepted)
    {
        QStandardItem *item = itemsModel.item(row, 0);
        item->setData(keyValueDialog->getKey(), Qt::EditRole);
        item->setData(paramId, Qt::UserRole);

        item = itemsModel.item(row, 1);
        item->setData(keyValueDialog->getType(), Qt::EditRole);

        item = itemsModel.item(row, 2);
        item->setData(keyValueDialog->getFileNameValue(), Qt::EditRole);
        item->setData(keyValueDialog->getFilePathValue(), Qt::UserRole);

        item = itemsModel.item(row, 3);
        item->setData(keyValueDialog->getDescription(), Qt::EditRole);

        return true;
    }

    return false;
}

QList<QVariant> KeyValueHandler::deleteSimpleRowModel(QTableView *tableView, QStandardItemModel &itemsModel)
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
