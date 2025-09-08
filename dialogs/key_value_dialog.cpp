#include "key_value_dialog.h"
#include "ui_key_value_dialog.h"

KeyValueDialog::KeyValueDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::KeyValueDialog)
{
    ui->setupUi(this);
}

KeyValueDialog::KeyValueDialog(QWidget *parent, QString key, QString value, QString description)
    : KeyValueDialog(parent)
{
    ui->keyEdit->setText(key);
    ui->valueEdit->setText(value);
    ui->descriptionEdit->setText(description);
}

KeyValueDialog::~KeyValueDialog()
{
    delete ui;
}

QString KeyValueDialog::getKey()
{
    return ui->keyEdit->text();
}

QString KeyValueDialog::getValue()
{
    return ui->valueEdit->text();
}

QString KeyValueDialog::getDescription()
{
    return ui->descriptionEdit->text();
}
