#include "controllereditwidget.h"
#include "ui_controllereditwidget.h"
#include "network/telnet.h"

ControllerEditWidget::ControllerEditWidget(QSharedPointer<Telnet> telnet, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControllerEditWidget)
{
    ui->setupUi(this);
    if (telnet) {
        ui->hostname->setText(telnet->hostname());
        ui->username->setText(telnet->username());
        ui->password->setText(telnet->password());
    }
}

ControllerEditWidget::ControllerEditWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControllerEditWidget)
{
    ui->setupUi(this);
}

ControllerEditWidget::~ControllerEditWidget()
{
    delete ui;
}

QString ControllerEditWidget::hostname() const
{
    return ui->hostname->text();
}

QString ControllerEditWidget::username() const
{
    return ui->username->text();
}

QString ControllerEditWidget::password() const
{
    return ui->password->text();
}
