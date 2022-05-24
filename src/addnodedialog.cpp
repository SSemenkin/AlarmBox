#include "addnodedialog.h"
#include "ui_addnodedialog.h"

#include <QMessageBox>

AddNodeDialog::AddNodeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddNodeDialog)
{
    ui->setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    ui->nodeName->setFocus();
}

AddNodeDialog::~AddNodeDialog()
{
    delete ui;
}

void AddNodeDialog::setDefaultType(int index)
{
    ui->nodeTypeCombo->setCurrentIndex(index);
}

void AddNodeDialog::setDefaultVendor(int index)
{
    ui->nodeVendorCombo->setCurrentIndex(index);
}

void AddNodeDialog::accept()
{
    QString name = ui->nodeName->text();
    QString dest = ui->nodeDest->text();
    int type = ui->nodeTypeCombo->currentIndex();
    int vendor = ui->nodeVendorCombo->currentIndex();

    if (!name.isEmpty()
    && !dest.isEmpty())
    {
        emit addNodeRequested(type, vendor, name, dest);
    } else {
        QMessageBox::warning(this, tr("Empty fields"), tr("Please fill all fields."));
    }

    QDialog::accept();
}
