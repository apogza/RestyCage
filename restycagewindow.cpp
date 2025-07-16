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

void RestyCageWindow::on_queryTabWidget_tabCloseRequested(int index)
{
    ui->queryTabWidget->removeTab(index);
}

void RestyCageWindow::on_newQueryBtn_clicked()
{
    addNewQuery();
}


void RestyCageWindow::on_addEnvironmentBtn_clicked()
{
    EnvironmentForm *environmentForm = new EnvironmentForm(ui->queryTabWidget);    
    int idx = ui->queryTabWidget->addTab(environmentForm, "New Environment");

    ui->queryTabWidget->setCurrentIndex(idx);
}

void RestyCageWindow::addNewQuery()
{
    QueryForm *queryForm = new QueryForm(ui->queryTabWidget);
    int idx = ui->queryTabWidget->addTab(queryForm, "New Query");

    ui->queryTabWidget->setCurrentIndex(idx);
}

void RestyCageWindow::initEnvironments()
{
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

void RestyCageWindow::on_envsTreeView_doubleClicked(const QModelIndex &index)
{
    QString fileName = envsModel.item(index.row(), index.column())->text();

    if (tabs.contains(fileName))
    {
        ui->queryTabWidget->setCurrentWidget(tabs[fileName]);
        return;
    }

    EnvironmentForm *environmentForm = new EnvironmentForm(ui->queryTabWidget);
    int idx = ui->queryTabWidget->addTab(environmentForm, fileName);
    environmentForm->initFromFile(fileName);

    tabs.insert(fileName, environmentForm);
    ui->queryTabWidget->setCurrentIndex(idx);
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

        //mainDir = QDir(reqDirPath);

        QString newCollectionName = nameDialog.getName();
        mainDir.mkdir(newCollectionName);
    }
}

