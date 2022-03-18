#include "addexceptiondialog.h"
#include "ui_addexceptiondialog.h"
#include "network/telnet.h"
#include "settings.h"

AddExceptionDialog::AddExceptionDialog(const QList<QSharedPointer<Telnet>> &controllerList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddExceptionDialog)
  , m_controllers(controllerList)
  , m_settings(Settings::instance())
{
    ui->setupUi(this);
    ui->comboBox->addItems({tr("CF Alarm"), tr("Manually blocked"), tr("Halted"), tr("Not works")});

    for (int i = 0; i < m_controllers.size(); ++i) {
        const QSharedPointer<Telnet> &controller = m_controllers.at(i);
        ui->comboBox_2->addItem(controller->isLoggedInNode() ? controller->parsedTitle() : controller->hostname());
    }
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &AddExceptionDialog::addException);
}

AddExceptionDialog::~AddExceptionDialog()
{
    delete ui;
}

void AddExceptionDialog::addException()
{

}
