#include "addexceptiondialog.h"
#include "ui_addexceptiondialog.h"
#include "network/telnet.h"
#include "settings.h"
#include <QMessageBox>

AddExceptionDialog::AddExceptionDialog(const QList<QSharedPointer<Telnet>> &controllerList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddExceptionDialog)
  , m_controllers(controllerList)
  , m_settings(Settings::instance())
{
    ui->setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    ui->alarmTypeCombo->addItems({tr("CF Alarm"), tr("Manually blocked"), tr("Halted"), tr("Not works")});

    for (int i = 0; i < m_controllers.size(); ++i) {
        const QSharedPointer<Telnet> &controller = m_controllers.at(i);
        ui->controllerCombo->addItem(controller->isLoggedInNode() ? controller->parsedTitle() : controller->hostname());
    }
}

AddExceptionDialog::~AddExceptionDialog()
{
    delete ui;
}

void AddExceptionDialog::accept()
{
    if (ui->object->text().isEmpty()) {
        QMessageBox::information(this, tr("Error"), tr("Cannot add exception. Object is empty."));
        return;
    }
    emit exceptionAdded(ui->controllerCombo->currentText(),
                        ui->object->text(),
                        ui->alarmTypeCombo->currentText());
    QDialog::accept();
}
