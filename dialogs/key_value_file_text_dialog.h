#ifndef KEY_VALUE_FILE_TEXT_DIALOG_H
#define KEY_VALUE_FILE_TEXT_DIALOG_H

#include <QDialog>

namespace Ui {
class KeyValueFileTextDialog;
}

class KeyValueFileTextDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KeyValueFileTextDialog(QWidget *parent = nullptr);
    explicit KeyValueFileTextDialog(QWidget *parent, QString &key, QString &type, QString &value, QString &description);
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

#endif // KEY_VALUE_FILE_TEXT_DIALOG_H
