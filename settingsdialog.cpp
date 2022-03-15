#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    ui->comboBox->addItems(QStringList{tr("English"), tr("Russian")});
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}
