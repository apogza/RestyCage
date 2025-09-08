#ifndef COLLECTION_TREEVIEW_MODEL_H
#define COLLECTION_TREEVIEW_MODEL_H

#include <QObject>

class CollectionTreeviewModel : public QObject
{
    Q_OBJECT
public:
    enum ItemType { Collection, Query };
    explicit CollectionTreeviewModel(int id, ItemType itemType, QObject *parent = nullptr);

    int id();
    ItemType itemType();


private:
    int m_id;
    ItemType m_itemType;
};

#endif // COLLECTION_TREEVIEW_MODEL_H
