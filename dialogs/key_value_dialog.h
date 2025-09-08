#ifndef KEY_VALUE_DIALOG_H
#define KEY_VALUE_DIALOG_H

#include <QDialog>

namespace Ui {
class KeyValueDialog;
}

class KeyValueDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KeyValueDialog(QWidget *parent = nullptr);
    explicit KeyValueDialog(QWidget *parent, QString key, QString value, QString description);
    ~KeyValueDialog();

    QString getKey();
    QString getValue();
    QString getDescription();

private:
    Ui::KeyValueDialog *ui;
};

#endif // KEY_VALUE_DIALOG_H
