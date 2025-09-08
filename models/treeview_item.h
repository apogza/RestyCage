#ifndef TREEVIEW_ITEM_H
#define TREEVIEW_ITEM_H

class TreeviewItem
{
public:
    enum ItemType {Collection, Query};
    TreeviewItem();

    TreeviewItem(ItemType type, int id);

    int id();
    ItemType itemType();

private:
    ItemType m_itemType;
    int m_id;

};


#endif // TREEVIEW_ITEM_H
