#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "settings.h"
#include "version.h"

#include <QSimpleUpdater.h>

#include <QApplication>
#include <QFontDialog>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
  , m_settings(*Settings::instance())
  , m_updater(QSimpleUpdater::getInstance())
{
    ui->setupUi(this);
    setupUpdater();
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    ui->comboBox->addItems(QStringList{tr("English"), tr("Russian")});

    if (m_settings.getLocale() != QLocale(QLocale::English)) {
        ui->comboBox->setCurrentIndex(1);
    }

    connect(ui->checkBox, &QCheckBox::stateChanged, ui->spinBox, &QSpinBox::setEnabled);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::applySettings);
    connect(ui->fontButton, &QPushButton::clicked, this, &SettingsDialog::chooseFont);

    connect(ui->checkForUpdatesButton, &QPushButton::clicked, this, [this] () {
        m_updater->checkForUpdates(m_settings.UPDATES_URL);
    });
    connect(m_updater, &QSimpleUpdater::checkingFinished, this, [this] (const QString &url) {
        m_settings.setLastUpdates(m_updater->getChangelog(url));
    });


    ui->checkBox->setChecked(m_settings.getIsAutoRefreshEnabled());
    ui->spinBox->setValue(m_settings.getRefreshPeriod());

    m_initValues.language = m_settings.getLocale();
    m_initValues.period   = m_settings.getRefreshPeriod();
    m_initValues.refresh  = m_settings.getIsAutoRefreshEnabled();

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

    if (m_initValues.language != m_settings.getLocale()) {
        emit localeChanged(m_settings.getLocale());
    }

    if (m_initValues.period != m_settings.getRefreshPeriod()) {
        emit periodChanged(m_settings.getRefreshPeriod());
    }

    if (m_initValues.refresh != m_settings.getIsAutoRefreshEnabled()) {
        emit autoRefreshChanged(m_settings.getIsAutoRefreshEnabled());
    }
}

void SettingsDialog::chooseFont() const
{
    QFontDialog dialog(qApp->font());
    connect(&dialog, &QFontDialog::fontSelected, this, [](const QFont& font){
        qApp->setFont(font);
    });
    dialog.exec();
}

void SettingsDialog::setupUpdater()
{
    m_updater->setModuleVersion(m_settings.UPDATES_URL, APPLICATION_VERSION);
    m_updater->setNotifyOnFinish(m_settings.UPDATES_URL, true);
    m_updater->setNotifyOnUpdate(m_settings.UPDATES_URL, true);
    m_updater->setUseCustomAppcast(m_settings.UPDATES_URL, false);
    m_updater->setDownloaderEnabled(m_settings.UPDATES_URL, true);
    m_updater->setMandatoryUpdate(m_settings.UPDATES_URL, true);
}
