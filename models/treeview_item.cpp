#include "treeview_item.h"

TreeviewItem::TreeviewItem()
{

}

TreeviewItem::TreeviewItem(ItemType itemType, int id)
{
    m_itemType = itemType;
    m_id = id;
}

int TreeviewItem::id()
{
    return m_id;
}

TreeviewItem::ItemType TreeviewItem::itemType()
{
    return m_itemType;
}


