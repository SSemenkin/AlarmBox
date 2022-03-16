#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "settings.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
  , m_settings(*Settings::instance())
{
    ui->setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    ui->comboBox->addItems(QStringList{tr("English"), tr("Russian")});

    if (m_settings.locale() != QLocale(QLocale::English)) {
        ui->comboBox->setCurrentIndex(1);
    }


    connect(ui->checkBox, &QCheckBox::stateChanged, ui->spinBox, &QSpinBox::setEnabled);
    ui->checkBox->setChecked(m_settings.autoRefreshEnabled());
    ui->spinBox->setValue(m_settings.period());

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::applySettings);

    m_initValues.language = m_settings.locale();
    m_initValues.period   = m_settings.period();
    m_initValues.refresh  = m_settings.autoRefreshEnabled();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::applySettings()
{
    m_settings.setLocale(ui->comboBox->currentIndex() == 0 ? QLocale(QLocale::English) : QLocale(QLocale::Russian));
    m_settings.setRefreshEnabled(ui->checkBox->isChecked());
    m_settings.setPeriod(ui->spinBox->value());


    if (m_initValues.language != m_settings.locale()) {
        emit localeChanged(m_settings.locale());
    }

    if (m_initValues.period != m_settings.period()) {
        emit periodChanged(m_settings.period());
    }

    if (m_initValues.refresh != m_settings.autoRefreshEnabled()) {
        emit autoRefreshChanged(m_settings.autoRefreshEnabled());
    }

}
