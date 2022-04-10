#include "addexceptiondialog.h"
#include "ui_addexceptiondialog.h"
#include "network/telnet.h"
#include "settings.h"
#include <QMessageBox>

AddExceptionDialog::AddExceptionDialog(const QList<QSharedPointer<Telnet>> &controllerList, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::AddExceptionDialog)
  , m_controllers(controllerList)
  , m_settings(Settings::instance())
{
    m_ui->setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    m_ui->alarmTypeCombo->addItems({tr("CF Alarm"), tr("Manually blocked"), tr("Halted"), tr("Not works")});

    for (int i = 0; i < m_controllers.size(); ++i) {
        const QSharedPointer<Telnet> &controller = m_controllers.at(i);
        m_ui->controllerCombo->addItem(controller->isLoggedInNode() ? controller->parsedTitle() : controller->hostname());
    }
}

AddExceptionDialog::~AddExceptionDialog()
{
    delete m_ui;
}

void AddExceptionDialog::accept()
{
    if (m_ui->object->text().isEmpty()) {
        QMessageBox::information(this, tr("Error"), tr("Cannot add exception. Object is empty."));
        return;
    }
    emit exceptionAdded(m_ui->controllerCombo->currentText(),
                        m_ui->object->text(),
                        QString::number(m_ui->alarmTypeCombo->currentIndex()));
    QDialog::accept();
}
