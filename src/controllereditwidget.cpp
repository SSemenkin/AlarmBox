#include "controllereditwidget.h"
#include "ui_controllereditwidget.h"
#include "network/telnet.h"

ControllerEditWidget::ControllerEditWidget(QSharedPointer<Telnet> telnet, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::ControllerEditWidget)
{
    m_ui->setupUi(this);
    if (telnet) {
        m_ui->hostname->setText(telnet->hostname());
        m_ui->username->setText(telnet->username());
        m_ui->password->setText(telnet->password());
    }
}

ControllerEditWidget::ControllerEditWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::ControllerEditWidget)
{
    m_ui->setupUi(this);
}

ControllerEditWidget::~ControllerEditWidget()
{
    delete m_ui;
}

QString ControllerEditWidget::hostname() const
{
    return m_ui->hostname->text();
}

QString ControllerEditWidget::username() const
{
    return m_ui->username->text();
}

QString ControllerEditWidget::password() const
{
    return m_ui->password->text();
}
