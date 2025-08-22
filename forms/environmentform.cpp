#include "environmentform.h"
#include "ui_environmentform.h"
#include "../environmentserializer.h"
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

void EnvironmentForm::initFromFile(QString &fileName)
{
    m_envName = QString(fileName);

    EnvironmentSerializer serializer;
    serializer.loadModelFromFile(fileName, m_envItemModel);
}

void EnvironmentForm::initFromDb(Environment &env)
{
    m_envName = QString(env.name());
    m_envId = env.id();

    QList<ParamValue> params = env.params();

    for (int i = 0; i < params.size(); i++)
    {
        QStandardItem *nameItem = new QStandardItem();
        nameItem->setText(params[i].getValue("name"));
        nameItem->setData(params[i].id().value(), Qt::UserRole);

        QStandardItem *valueItem = new QStandardItem();
        valueItem->setText(params[i].getValue("value"));

        QStandardItem *descriptionItem = new QStandardItem();
        descriptionItem->setText(params[i].getValue("description"));

        m_envItemModel.setItem(i, 0, nameItem);
        m_envItemModel.setItem(i, 1, valueItem);
        m_envItemModel.setItem(i, 2, descriptionItem);
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

    for (int i = 0; i < userData.size(); i++)
    {
        m_deletedParams.append(userData[i].toInt());
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

        env.addParam(ParamValue(id, nameItem->text(), valueItem->text(), descriptionItem->text()));
    }

    m_db.saveEnv(env);

    m_deletedParams.clear();
    emit changedName(this, m_envName);
}

