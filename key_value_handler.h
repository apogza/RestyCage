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

    bool addSimpleRowModel(QWidget *widget, QStandardItemModel &itemsModel);
    bool addRowModel(QWidget *widget, QStandardItemModel &itemsModel);
    bool editSimpleRowModel(QWidget *widget, QStandardItemModel &itemsModel, int row, int column);
    bool editRowModel(QWidget *widget, QStandardItemModel &itemsModel, int row, int column);
    QList<QVariant> deleteSimpleRowModel(QTableView *tableView, QStandardItemModel &itemsModel);
};

#endif // KEY_VALUE_HANDLER_H
