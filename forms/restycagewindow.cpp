#include "restycagewindow.h"
#include "ui_restycagewindow.h"
#include "environmentform.h"
#include "queryform.h"
#include "../dialogs/namedialog.h"
#include "../constants.h"

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
    int idx = ui->tabWidget->addTab(queryForm, "New Query*");

    ui->tabWidget->setCurrentIndex(idx);
}

void RestyCageWindow::initEnvironments()
{
    m_envsModel.clear();

    QList<Environment> envs = m_db.getEnvs();

    for (int i = 0; i < envs.size(); i++)
    {
        QStandardItem *item = new QStandardItem();
        item->setText(envs[i].name());
        item->setData(envs[i].id().value(), Qt::UserRole);
        m_envsModel.appendRow(item);
    }
}

void RestyCageWindow::initCollections()
{
    QDir collectionDir(collectionDirPath);
    traverseCollectionsDir(collectionDir);

    ui->collectionsTreeView->setModel(&m_collectionsModel);
}

void RestyCageWindow::traverseCollectionsDir(QDir currentDir)
{
    QFileInfoList entries = currentDir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    foreach (QFileInfo info, entries)
    {
        QStandardItem *item = new QStandardItem();
        item->setText(info.baseName());
        m_collectionsModel.appendRow(item);

        if (info.isDir())
        {
            traverseCollectionsDir(QDir(info.filePath()));
        }
    }
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

    initEnvironments();
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
    NameDialog nameDialog;
    int dialogResult = nameDialog.exec();

    if (dialogResult == QDialog::Accepted)
    {
        QDir mainDir(collectionDirPath);

        if (!mainDir.exists())
        {
            QDir().mkdir(collectionDirPath);
        }

        QString newCollectionName = nameDialog.getName();
        mainDir.mkdir(newCollectionName);
    }
}


void RestyCageWindow::on_delEnvBtn_clicked()
{
    QDir envDir(envDirPath);
    QModelIndex idx = ui->envsTreeView->currentIndex();

    QStandardItem *selectedEnvItem = m_envsModel.itemFromIndex(idx);
    QString fileName = QString("%1.json").arg(selectedEnvItem->text());
    bool delResult = envDir.remove(fileName);

    if (delResult)
    {
        initEnvironments();
    }
}

