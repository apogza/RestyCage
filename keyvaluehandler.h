#ifndef KEYVALUEHANDLER_H
#define KEYVALUEHANDLER_H

#include <QObject>
#include <QWidget>
#include <QStandardItemModel>
#include <QTableView>

class KeyValueHandler : public QObject
{
    Q_OBJECT
public:
    explicit KeyValueHandler(QObject *parent = nullptr);

    void addRowModel(QWidget *widget, QStandardItemModel &itemsModel);
    void editRowModel(QWidget *widget, QStandardItemModel &itemsModel, int row, int column);
    void deleteRowModel(QTableView *tableView, QStandardItemModel &itemsModel);
};

#endif // KEYVALUEHANDLER_H
