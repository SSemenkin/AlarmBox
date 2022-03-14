#include "editcontrollerdialog.h"
#include "ui_editcontrollerdialog.h"
#include "controllereditwidget.h"
#include "network/telnet.h"

EditControllerDialog::EditControllerDialog(QSharedPointer<Telnet> telnet, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditControllerDialog),
    m_d(new ControllerEditWidget(telnet, this)),
    m_telnet(telnet)
{
    ui->setupUi(this);

    ui->verticalLayout->addWidget(m_d);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &EditControllerDialog::onAcceptPressed);
}

EditControllerDialog::~EditControllerDialog()
{
    delete ui;
}

void EditControllerDialog::onAcceptPressed()
{
    if (!m_telnet.isNull()) {
        m_telnet->setHostname(m_d->hostname());
        m_telnet->setUsername(m_d->username());
        m_telnet->setPassword(m_d->password());
        m_telnet->connectToNode();
    }
}
