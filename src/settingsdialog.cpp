#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "settings.h"

#include <QApplication>
#include <QFontDialog>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SettingsDialog)
  , m_settings(*Settings::instance())
{
    m_ui->setupUi(this);
    m_ui->themeCombo->addItems({tr("Light"), tr("Dark")});
    m_ui->themeCombo->setCurrentIndex(m_settings.getThemeIndex());
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    m_ui->comboBox->addItems(QStringList{tr("English"), tr("Russian")});

    if (m_settings.getLocale() != QLocale(QLocale::English)) {
        m_ui->comboBox->setCurrentIndex(1);
    }

    connect(m_ui->checkBox, &QCheckBox::stateChanged, m_ui->spinBox, &QSpinBox::setEnabled);
    connect(m_ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::applySettings);
    connect(m_ui->fontButton, &QPushButton::clicked, this, &SettingsDialog::chooseFont);


    m_ui->checkBox->setChecked(m_settings.getIsAutoRefreshEnabled());
    m_ui->spinBox->setValue(m_settings.getRefreshPeriod());
    m_ui->checkBox_2->setChecked(m_settings.getIsDVDVideoEnabled());

    m_initValues.language   = m_settings.getLocale();
    m_initValues.period     = m_settings.getRefreshPeriod();
    m_initValues.refresh    = m_settings.getIsAutoRefreshEnabled();
    m_initValues.themeIndex = m_settings.getThemeIndex();
    m_initValues.font       = m_settings.getFont();
    m_initValues.dvdVideo   = m_settings.getIsDVDVideoEnabled();

}

SettingsDialog::~SettingsDialog()
{
    delete m_ui;
}

void SettingsDialog::applySettings()
{
    m_settings.setLocale(m_ui->comboBox->currentIndex() == 0 ? QLocale(QLocale::English) : QLocale(QLocale::Russian));
    m_settings.setAutoRefreshEnabled(m_ui->checkBox->isChecked());
    m_settings.setRefreshPeriod(m_ui->spinBox->value());
    m_settings.setThemeIndex(m_ui->themeCombo->currentIndex());
    m_settings.setDVDVideoEnabled(m_ui->checkBox_2->isChecked());

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

    if (m_initValues.dvdVideo != m_settings.getIsDVDVideoEnabled()) {
        emit DVDVideoStateChanged(m_settings.getIsDVDVideoEnabled());
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
