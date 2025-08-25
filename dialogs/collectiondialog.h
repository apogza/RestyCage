#ifndef COLLECTIONDIALOG_H
#define COLLECTIONDIALOG_H

#include <QDialog>
#include "../db/collection.h"

namespace Ui {
class CollectionDialog;
}

class CollectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CollectionDialog(QWidget *parent = nullptr);
    explicit CollectionDialog(QList<Collection> &collectionList, QWidget *parent = nullptr);
    ~CollectionDialog();
    void setCollectionList(QList<Collection>& collectionList);

    QString name();
    std::optional<int> parent();

private:
    Ui::CollectionDialog *ui;
    QList<Collection> m_collectionsList;
};

#endif // COLLECTIONDIALOG_H
