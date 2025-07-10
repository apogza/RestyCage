#include "restycagewindow.h"
#include "ui_restycagewindow.h"
#include "queryform.h"
#include "environmentform.h"

RestyCageWindow::RestyCageWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RestyCageWindow)
{
    ui->setupUi(this);
    ui->splitter->setSizes({100, 500});
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
    QueryForm *queryForm = new QueryForm(ui->queryTabWidget);
    ui->queryTabWidget->addTab(queryForm, "New Query");
}

void RestyCageWindow::on_newCollectionBtn_clicked()
{
    EnvironmentForm *environmentForm = new EnvironmentForm(ui->queryTabWidget);
    ui->queryTabWidget->addTab(environmentForm, "New Environment");
}

void RestyCageWindow::on_collectionsToolButton_clicked()
{
    ui->menuStackedWidget->setCurrentIndex(0);
}

void RestyCageWindow::on_envToolButton_clicked()
{
    ui->menuStackedWidget->setCurrentIndex(1);
}

