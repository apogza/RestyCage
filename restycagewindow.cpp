#include "restycagewindow.h"
#include "ui_restycagewindow.h"
#include "queryform.h"
#include "environmentform.h"
#include "namedialog.h"
#include "constants.h"

#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>

RestyCageWindow::RestyCageWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RestyCageWindow)
{
    ui->setupUi(this);
    ui->splitter->setSizes({200, 500});
    ui->envsTreeView->setModel(&envsModel);

    addNewQuery();
    initCollections();
    initEnvironments();
}

RestyCageWindow::~RestyCageWindow()
{
    delete ui;
}

void RestyCageWindow::on_tabWidget_tabCloseRequested(int index)
{
    QString tabTitle = ui->tabWidget->tabText(index);
    tabs.remove(tabTitle);

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
    envsModel.clear();

    QDir envDir(envDirPath);
    QFileInfoList entries = envDir.entryInfoList(QDir::Files);

    foreach (QFileInfo info, entries)
    {
        QStandardItem *item = new QStandardItem();
        item->setText(info.baseName());

        envsModel.appendRow(item);
    }
}

void RestyCageWindow::initCollections()
{
    QDir collectionDir(collectionDirPath);
    traverseCollectionsDir(collectionDir);

    ui->collectionsTreeView->setModel(&collectionsModel);
}

void RestyCageWindow::traverseCollectionsDir(QDir currentDir)
{
    QFileInfoList entries = currentDir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    foreach (QFileInfo info, entries)
    {
        QStandardItem *item = new QStandardItem();
        item->setText(info.baseName());
        collectionsModel.appendRow(item);

        if (info.isDir())
        {
            traverseCollectionsDir(QDir(info.filePath()));
        }
    }
}

void RestyCageWindow::onTabHasChangedName(QWidget *widget, QString newTitle)
{
    int idx = ui->tabWidget->indexOf(widget);
    QString oldTitle = ui->tabWidget->tabText(idx);

    ui->tabWidget->setTabText(idx, newTitle);

    tabs.remove(oldTitle);
    tabs.insert(newTitle, widget);

    initEnvironments();
}

void RestyCageWindow::onTabHasBeenModified(QWidget *widget)
{
    int idx = ui->tabWidget->indexOf(widget);
    QString oldTitle = ui->tabWidget->tabText(idx);
    QString newTitle = QString("%1%2").arg(oldTitle, "*");

    ui->tabWidget->setTabText(idx, newTitle);

    tabs.remove(oldTitle);
    tabs.insert(newTitle, widget);
}

void RestyCageWindow::on_envsTreeView_doubleClicked(const QModelIndex &index)
{
    QString fileName = envsModel.item(index.row(), index.column())->text();

    if (tabs.contains(fileName))
    {
        ui->tabWidget->setCurrentWidget(tabs[fileName]);
        return;
    }

    EnvironmentForm *environmentForm = new EnvironmentForm(ui->tabWidget);
    int idx = ui->tabWidget->addTab(environmentForm, fileName);
    environmentForm->initFromFile(fileName);

    connect(environmentForm, &EnvironmentForm::changedName, this, &RestyCageWindow::onTabHasChangedName);
    connect(environmentForm, &EnvironmentForm::hasBeenModified, this, &RestyCageWindow::onTabHasBeenModified);

    tabs.insert(fileName, environmentForm);
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

    QStandardItem *selectedEnvItem = envsModel.itemFromIndex(idx);
    QString fileName = QString("%1.json").arg(selectedEnvItem->text());
    bool delResult = envDir.remove(fileName);

    if (delResult)
    {
        initEnvironments();
    }
}

