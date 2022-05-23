#include "addnodedialog.h"
#include "ui_addnodedialog.h"

AddNodeDialog::AddNodeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddNodeDialog)
{
    ui->setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
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
