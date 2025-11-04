#include "resty_cage_window.h"
#include "ui_resty_cage_window.h"
#include "environment_form.h"
#include "query_form.h"
#include "../dialogs/collection_dialog.h"
#include "../constants.h"
#include "../db/collection.h"
#include "../models/treeview_item.h"

#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>

RestyCageWindow::RestyCageWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RestyCageWindow)
{
    ui->setupUi(this);
    ui->splitter->setSizes({200, 500});
    ui->envsTreeView->setModel(&m_envsModel);

    addNewQuery();
    initAppDataFolder();
    initCollections();
    initEnvironments();
    ui->toolBox->setCurrentIndex(0);
}

RestyCageWindow::~RestyCageWindow()
{
    delete ui;
}

void RestyCageWindow::on_tabWidget_tabCloseRequested(int index)
{
    QString tabTitle = ui->tabWidget->tabText(index);
    m_tabs.remove(tabTitle);

    ui->tabWidget->removeTab(index);
}

void RestyCageWindow::on_newQueryBtn_clicked()
{
    addNewQuery();
}

void RestyCageWindow::on_addEnvironmentBtn_clicked()
{
    EnvironmentForm *environmentForm = new EnvironmentForm(ui->tabWidget);
    int idx = ui->tabWidget->addTab(environmentForm, "New Environment*");

    ui->tabWidget->setCurrentIndex(idx);

    connect(environmentForm, &EnvironmentForm::changedName, this, &RestyCageWindow::onTabHasChangedName);
    connect(environmentForm, &EnvironmentForm::hasBeenModified, this, &RestyCageWindow::onTabHasBeenModified);
}

void RestyCageWindow::addNewQuery()
{
    QueryForm *queryForm = new QueryForm(ui->tabWidget);

    connect(queryForm, &QueryForm::changedName, this, &RestyCageWindow::onTabHasChangedName);
    connect(queryForm, &QueryForm::hasBeenModified, this, &RestyCageWindow::onTabHasBeenModified);

    int idx = ui->tabWidget->addTab(queryForm, "New Query*");

    ui->tabWidget->setCurrentIndex(idx);
}

void RestyCageWindow::initEnvironments()
{
    m_envsModel.clear();

    QList<Environment> envs = m_db.getEnvs();

    for (Environment &env : envs)
    {
        QStandardItem *item = new QStandardItem();
        item->setText(env.name());
        item->setData(env.id().value(), Qt::UserRole);
        m_envsModel.appendRow(item);
    }
}

void RestyCageWindow::initCollections()
{
    m_collectionsModel.clear();

    QList<Collection> collections = m_db.getCollections(true);
    QMap<int, QStandardItem*> collectionsItemMap;

    // create the hierarchy for the collections
    for (Collection &currentCollection : collections)
    {
        QStandardItem *collectionItem = new QStandardItem();
        collectionItem->setText(currentCollection.name());

        TreeviewItem itemData(TreeviewItem::ItemType::Collection, currentCollection.id().value());
        QVariant data;
        data.setValue(itemData);

        collectionItem->setData(data, Qt::UserRole);
        collectionItem->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::FolderOpen));

        std::optional<int> parentId = currentCollection.parent();
        if (parentId.has_value() && collectionsItemMap.contains(parentId.value()))
        {
            QStandardItem *parentItem = collectionsItemMap[parentId.value()];
            parentItem->setChild(parentItem->rowCount(), collectionItem);
        }
        else
        {
            m_collectionsModel.appendRow(collectionItem);
        }

        collectionsItemMap.insert(currentCollection.id().value(), collectionItem);
    }

    // add the queries
    for (Collection &currentCollection : collections)
    {
        QList<Query> &queries = currentCollection.queries();
        QStandardItem *collectionItem = collectionsItemMap[currentCollection.id().value()];

        for (Query &query : queries)
        {
            QStandardItem *queryItem = new QStandardItem();
            queryItem->setText(query.name());
            TreeviewItem itemData(TreeviewItem::ItemType::Query, query.id().value());
            QVariant data;
            data.setValue(itemData);

            queryItem->setData(data, Qt::UserRole);
            queryItem->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew));
            collectionItem->setChild(collectionItem->rowCount(), queryItem);
        }
    }

    ui->collectionsTreeView->setModel(&m_collectionsModel);
    ui->collectionsTreeView->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
    ui->collectionsTreeView->expandAll();
}

void RestyCageWindow::initAppDataFolder()
{
    QDir dataFolder = appDataFolder;

    if (!dataFolder.exists())
    {
        QDir().mkdir(appDataFolder);
    }
}

void RestyCageWindow::onTabHasChangedName(QWidget *widget, QString newTitle)
{
    int idx = ui->tabWidget->indexOf(widget);
    QString oldTitle = ui->tabWidget->tabText(idx);

    ui->tabWidget->setTabText(idx, newTitle);

    m_tabs.remove(oldTitle);
    m_tabs.insert(newTitle, widget);

    QueryForm *queryForm = dynamic_cast<QueryForm*>(widget);
    EnvironmentForm *envForm = dynamic_cast<EnvironmentForm*>(widget);

    if (queryForm)
    {
        initCollections();
    }

    if (envForm)
    {
        initEnvironments();
    }
}

void RestyCageWindow::onTabHasBeenModified(QWidget *widget)
{
    int idx = ui->tabWidget->indexOf(widget);
    QString oldTitle = ui->tabWidget->tabText(idx);
    QString newTitle = QString("%1%2").arg(oldTitle, "*");

    ui->tabWidget->setTabText(idx, newTitle);

    m_tabs.remove(oldTitle);
    m_tabs.insert(newTitle, widget);
}

void RestyCageWindow::on_envsTreeView_doubleClicked(const QModelIndex &index)
{
    QStandardItem *selectedEnvItem = m_envsModel.item(index.row(), index.column());
    QString envName = selectedEnvItem->text();

    int envId = selectedEnvItem->data(Qt::UserRole).toInt();
    std::optional<Environment> env = m_db.getEnv(envId);

    if (!env.has_value())
    {
        return;
    }

    if (m_tabs.contains(envName))
    {
        ui->tabWidget->setCurrentWidget(m_tabs[envName]);
        return;
    }

    EnvironmentForm *environmentForm = new EnvironmentForm(ui->tabWidget);
    int idx = ui->tabWidget->addTab(environmentForm, envName);

    environmentForm->initFromDb(env.value());

    connect(environmentForm, &EnvironmentForm::changedName, this, &RestyCageWindow::onTabHasChangedName);
    connect(environmentForm, &EnvironmentForm::hasBeenModified, this, &RestyCageWindow::onTabHasBeenModified);

    m_tabs.insert(envName, environmentForm);
    ui->tabWidget->setCurrentIndex(idx);
}

void RestyCageWindow::on_newCollectionBtn_clicked()
{
    QList<Collection> collectionsList = m_db.getCollections();
    CollectionDialog collectionDialog;
    collectionDialog.setCollectionList(collectionsList);

    int dialogResult = collectionDialog.exec();

    if (dialogResult == QDialog::Accepted)
    {
        QString collectionName = collectionDialog.name();
        std::optional<int> parent = collectionDialog.collectionId();
        Collection newCollection(collectionName, parent);
        m_db.saveCollection(newCollection);

        initCollections();
    }
}


void RestyCageWindow::on_delEnvBtn_clicked()
{
    QModelIndex idx = ui->envsTreeView->currentIndex();
    QStandardItem *selectedEnvItem = m_envsModel.itemFromIndex(idx);

    int envId = selectedEnvItem->data(Qt::UserRole).toInt();
    std::optional<Environment> env = m_db.getEnv(envId);

    bool delResult = m_db.deleteEnv(envId);

    if (delResult)
    {
        initEnvironments();
    }
}

void RestyCageWindow::on_removeCollectionBtn_clicked()
{
    QModelIndex idx = ui->collectionsTreeView->currentIndex();
    QStandardItem *selectedCollectionItem = m_collectionsModel.itemFromIndex(idx);

    TreeviewItem itemData = selectedCollectionItem->data(Qt::UserRole).value<TreeviewItem>();

    bool delResult = false;

    if (itemData.itemType() == TreeviewItem::ItemType::Collection)
    {
        delResult = m_db.deleteCollection(itemData.id());
    }

    if (itemData.itemType() == TreeviewItem::ItemType::Query)
    {
        delResult = m_db.deleteQuery(itemData.id());
    }

    if (delResult)
    {
        initCollections();
    }
}


void RestyCageWindow::on_collectionsTreeView_doubleClicked(const QModelIndex &index)
{
    QStandardItem *selectedCollectionItem = m_collectionsModel.itemFromIndex(index);
    TreeviewItem itemData = selectedCollectionItem->data(Qt::UserRole).value<TreeviewItem>();

    if (itemData.itemType() == TreeviewItem::Query)
    {
        std::optional<Query> query = m_db.getQuery(itemData.id());

        if (!query.has_value())
        {
            return;
        }

        QueryForm *queryForm = new QueryForm(this);

        connect(queryForm, &QueryForm::changedName, this, &RestyCageWindow::onTabHasChangedName);
        connect(queryForm, &QueryForm::hasBeenModified, this, &RestyCageWindow::onTabHasBeenModified);

        int idx = ui->tabWidget->addTab(queryForm, "");
        ui->tabWidget->setCurrentIndex(idx);

        queryForm->initFromDb(query.value());
    }
}

