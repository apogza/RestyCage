#ifndef NAME_DIALOG_H
#define NAME_DIALOG_H

#include <QDialog>

namespace Ui {
class NameDialog;
}

class NameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NameDialog(QWidget *parent = nullptr);
    ~NameDialog();

    QString getName();

private:
    Ui::NameDialog *ui;
};

#endif // NAME_DIALOG_H
