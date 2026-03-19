#include "key_value_file_text_dialog.h"
#include "ui_key_value_file_text_dialog.h"
#include <QFileDialog>

KeyValueFileTextDialog::KeyValueFileTextDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::KeyValueFileTextDialog)
{
    ui->setupUi(this);
}

KeyValueFileTextDialog::KeyValueFileTextDialog(QWidget *parent, QString &key, QString &type, QString &value, QString &description)
    : KeyValueFileTextDialog(parent)
{
    ui->keyEdit->setText(key);
    ui->typeComboBox->setCurrentText(type);
    if (type == "File")
    {
        QFileInfo fileInfo(value);
        filePath = value;
        fileName = fileInfo.fileName();

        ui->filepathLbl->setText(this->fileName);
    }
    else
    {
        ui->valueTextEdit->setText(value);
    }


    ui->descriptionEdit->setText(description);
}

KeyValueFileTextDialog::~KeyValueFileTextDialog()
{
    delete ui;
}

QString KeyValueFileTextDialog::getKey()
{
    return ui->keyEdit->text();
}

QString KeyValueFileTextDialog::getType()
{
    return ui->typeComboBox->currentText();
}

QString KeyValueFileTextDialog::getTextValue()
{
    return ui->valueTextEdit->text();
}

QString KeyValueFileTextDialog::getFileNameValue()
{
    return fileName;
}

QString KeyValueFileTextDialog::getFilePathValue()
{
    return filePath;
}

QString KeyValueFileTextDialog::getDescription()
{
    return ui->descriptionEdit->text();
}

void KeyValueFileTextDialog::on_typeComboBox_currentIndexChanged(int index)
{
    ui->stackedWidget->setCurrentIndex(index);
}

void KeyValueFileTextDialog::on_selectFileBtn_clicked()
{
    QFileDialog fileDialog;
    fileDialog.setFileMode(QFileDialog::AnyFile);

    int result = fileDialog.exec();

    if (result == QDialog::Accepted)
    {
        QStringList filePaths = fileDialog.selectedFiles();
        QString filePath = filePaths.at(0);


        QFileInfo fileInfo(filePath);

        this->filePath = filePath;
        fileName = fileInfo.fileName();

        ui->filepathLbl->setText(fileName);
    }
}

