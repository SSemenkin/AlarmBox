#include "addcontrollerdialog.h"
#include "ui_addcontrollerdialog.h"
#include "controllereditwidget.h"

#include <QMessageBox>

AddControllerDialog::AddControllerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddControllerDialog),
    m_d(new ControllerEditWidget(this))

{
    ui->setupUi(this);
    ui->verticalLayout->addWidget(m_d);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &AddControllerDialog::onAcceptPressed);
}

AddControllerDialog::~AddControllerDialog()
{
    delete ui;
}

void AddControllerDialog::onAcceptPressed()
{
    if (!m_d->hostname().isEmpty() &&
        !m_d->username().isEmpty() &&
        !m_d->password().isEmpty()) {
            emit newController(m_d->hostname(),
                               m_d->username(),
                               m_d->password());
    } else {
        QMessageBox::information(this, tr("Error add..."), tr("Fill empty fields."));
    }
    close();
}
