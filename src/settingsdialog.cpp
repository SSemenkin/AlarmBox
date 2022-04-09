#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "settings.h"

#include <QApplication>
#include <QFontDialog>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
  , m_settings(*Settings::instance())
{
    ui->setupUi(this);
    ui->themeCombo->addItems({tr("Light"), tr("Dark")});
    ui->themeCombo->setCurrentIndex(m_settings.getThemeIndex());
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    ui->comboBox->addItems(QStringList{tr("English"), tr("Russian")});

    if (m_settings.getLocale() != QLocale(QLocale::English)) {
        ui->comboBox->setCurrentIndex(1);
    }

    connect(ui->checkBox, &QCheckBox::stateChanged, ui->spinBox, &QSpinBox::setEnabled);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::applySettings);
    connect(ui->fontButton, &QPushButton::clicked, this, &SettingsDialog::chooseFont);


    ui->checkBox->setChecked(m_settings.getIsAutoRefreshEnabled());
    ui->spinBox->setValue(m_settings.getRefreshPeriod());

    m_initValues.language   = m_settings.getLocale();
    m_initValues.period     = m_settings.getRefreshPeriod();
    m_initValues.refresh    = m_settings.getIsAutoRefreshEnabled();
    m_initValues.themeIndex = m_settings.getThemeIndex();
    m_initValues.font       = m_settings.getFont();

}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::applySettings()
{
    m_settings.setLocale(ui->comboBox->currentIndex() == 0 ? QLocale(QLocale::English) : QLocale(QLocale::Russian));
    m_settings.setAutoRefreshEnabled(ui->checkBox->isChecked());
    m_settings.setRefreshPeriod(ui->spinBox->value());
    m_settings.setThemeIndex(ui->themeCombo->currentIndex());

    if (m_initValues.language != m_settings.getLocale()) {
        emit localeChanged(m_settings.getLocale());
    }

    if (m_initValues.period != m_settings.getRefreshPeriod()) {
        emit periodChanged(m_settings.getRefreshPeriod());
    }

    if (m_initValues.refresh != m_settings.getIsAutoRefreshEnabled()) {
        emit autoRefreshChanged(m_settings.getIsAutoRefreshEnabled());
    }

    if (m_initValues.themeIndex != m_settings.getThemeIndex()) {
        emit themeChanged(m_settings.getThemeIndex());
    }

    if (m_initValues.font != m_settings.getFont()) {
        m_settings.setFont(m_initValues.font);
        emit fontChanged(m_settings.getFont());
    }
}

void SettingsDialog::chooseFont()
{
    QFontDialog dialog(qApp->font());
    connect(&dialog, &QFontDialog::fontSelected, this,[this](const QFont &font) -> void {
        this->m_initValues.font = font;
    });
    dialog.exec();
}
