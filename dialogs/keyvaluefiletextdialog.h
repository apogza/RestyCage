#ifndef KEYVALUEFILETEXTDIALOG_H
#define KEYVALUEFILETEXTDIALOG_H

#include <QDialog>

namespace Ui {
class KeyValueFileTextDialog;
}

class KeyValueFileTextDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KeyValueFileTextDialog(QWidget *parent = nullptr);
    ~KeyValueFileTextDialog();

    QString getKey();
    QString getType();
    QString getTextValue();
    QString getFileNameValue();
    QString getFilePathValue();
    QString getDescription();

private slots:
    void on_typeComboBox_currentIndexChanged(int index);
    void on_selectFileBtn_clicked();

private:
    Ui::KeyValueFileTextDialog *ui;
    QString filePath;
    QString fileName;
};

#endif // KEYVALUEFILETEXTDIALOG_H
