#include "collection_treeview_model.h"

CollectionTreeviewModel::CollectionTreeviewModel(int id, ItemType itemType, QObject *parent)
    : QObject{parent}
{
    m_id = id;
    m_itemType = itemType;
}

int CollectionTreeviewModel::id()
{
    return m_id;
}

CollectionTreeviewModel::ItemType CollectionTreeviewModel::itemType()
{
    return m_itemType;
}
