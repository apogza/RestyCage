#ifndef KEY_VALUE_HANDLER_H
#define KEY_VALUE_HANDLER_H

#include <QObject>
#include <QWidget>
#include <QStandardItemModel>
#include <QTableView>

class KeyValueHandler : public QObject
{
    Q_OBJECT
public:
    explicit KeyValueHandler(QObject *parent = nullptr);

    bool addRowModel(QWidget *widget, QStandardItemModel &itemsModel);
    bool editRowModel(QWidget *widget, QStandardItemModel &itemsModel, int row, int column);
    QList<QVariant> deleteRowModel(QTableView *tableView, QStandardItemModel &itemsModel);
};

#endif // KEY_VALUE_HANDLER_H
