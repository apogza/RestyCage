#include "name_dialog.h"
#include "ui_name_dialog.h"

NameDialog::NameDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NameDialog)
{
    ui->setupUi(this);
}

NameDialog::~NameDialog()
{
    delete ui;
}

QString NameDialog::getName()
{
    return ui->nameLineEdit->text();
}
