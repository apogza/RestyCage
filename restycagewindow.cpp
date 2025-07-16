#include "restycagewindow.h"
#include "ui_restycagewindow.h"
#include "queryform.h"
#include "environmentform.h"
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
    ui->envsTreeView->setModel(&envModel);

    addNewQuery();
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

void RestyCageWindow::on_newCollectionBtn_clicked()
{

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

        envModel.appendRow(item);
    }
}


void RestyCageWindow::on_envsTreeView_doubleClicked(const QModelIndex &index)
{
    QString fileName = envModel.item(index.row(), index.column())->text();

    EnvironmentForm *environmentForm = new EnvironmentForm(ui->queryTabWidget);
    int idx = ui->queryTabWidget->addTab(environmentForm, "New Environment");
    environmentForm->initFromFile(fileName);

    ui->queryTabWidget->setCurrentIndex(idx);
}

