#include "restycagewindow.h"
#include "ui_restycagewindow.h"

RestyCageWindow::RestyCageWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RestyCageWindow)
{
    ui->setupUi(this);

    queryForm = new QueryForm(this);
    setCentralWidget(queryForm);
}

RestyCageWindow::~RestyCageWindow()
{
    delete ui;
}
