#include "environmentform.h"
#include "ui_environmentform.h"
#include "../constants.h"
#include "../db/paramvalue.h"
#include "../db/environment.h"
#include "../dialogs/namedialog.h"

#include <QVariant>

EnvironmentForm::EnvironmentForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EnvironmentForm)
{
    ui->setupUi(this);
    m_keyValueHandler = new KeyValueHandler(this);

    initModel();
}

EnvironmentForm::~EnvironmentForm()
{
    delete ui;
}

void EnvironmentForm::initFromDb(Environment &env)
{
    m_envName = QString(env.name());
    m_envId = env.id();

    QList<ParamValue> &params = env.params();

    int i = 0;

    for (ParamValue &param: params)
    {
        QStandardItem *nameItem = new QStandardItem();
        nameItem->setText(param.getValue("name"));
        nameItem->setData(param.id().value(), Qt::UserRole);

        QStandardItem *valueItem = new QStandardItem();
        valueItem->setText(param.getValue("value"));

        QStandardItem *descriptionItem = new QStandardItem();
        descriptionItem->setText(param.getValue("description"));

        m_envItemModel.setItem(i, 0, nameItem);
        m_envItemModel.setItem(i, 1, valueItem);
        m_envItemModel.setItem(i, 2, descriptionItem);
        i++;
    }
}

void EnvironmentForm::initModel()
{
    m_envItemModel.insertColumns(0, 3);
    m_envItemModel.setHeaderData(0, Qt::Horizontal, QObject::tr(keyHeader));
    m_envItemModel.setHeaderData(1, Qt::Horizontal, QObject::tr(valueHeader));
    m_envItemModel.setHeaderData(2, Qt::Horizontal, QObject::tr(descriptionHeader));

    ui->tableView->setModel(&m_envItemModel);
}

void EnvironmentForm::on_addEnvironmentBtn_clicked()
{
    bool result = m_keyValueHandler->addRowModel(this, m_envItemModel);

    if (result)
    {
        emit hasBeenModified(this);
    }
}

void EnvironmentForm::on_tableView_doubleClicked(const QModelIndex &index)
{
    bool result = m_keyValueHandler->editRowModel(this, m_envItemModel, index.row(), index.column());

    if (result)
    {
        emit hasBeenModified(this);
    }
}

void EnvironmentForm::on_removeEnvironmentBtn_clicked()
{
    QList<QVariant> userData = m_keyValueHandler->deleteRowModel(ui->tableView, m_envItemModel);

    for (QVariant &data : userData)
    {
        m_deletedParams.append(data.toInt());
    }

    emit hasBeenModified(this);
}

void EnvironmentForm::on_saveEnvironmentBtn_clicked()
{
    if (m_envName.isEmpty())
    {
        NameDialog nameDialog;
        int dialogResult = nameDialog.exec();

        if (dialogResult == QDialog::Accepted)
        {
            m_envName = nameDialog.getName();
        }
    }

    Environment env;
    env.setName(m_envName);
    env.setDeletedParams(m_deletedParams);

    if (m_envId.has_value())
    {
        env.setId(m_envId.value());
    }

    for(int i = 0; i < m_envItemModel.rowCount(); i++)
    {
        QStandardItem *nameItem = m_envItemModel.item(i, 0);
        QStandardItem *valueItem = m_envItemModel.item(i, 1);
        QStandardItem *descriptionItem = m_envItemModel.item(i, 2);

        std::optional<int> id;
        QVariant paramId = nameItem->data(Qt::UserRole);

        if (!paramId.isNull())
        {
            id = paramId.toInt();
        }
        ParamValue newParam(id, nameItem->text(), valueItem->text(), descriptionItem->text());
        env.addParam(newParam);
    }

    m_db.saveEnv(env);

    m_deletedParams.clear();
    emit changedName(this, m_envName);
}

