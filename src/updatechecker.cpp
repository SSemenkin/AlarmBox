#include "updatechecker.h"
#include "version.h"
#include "settings.h"
#include <QSimpleUpdater.h>
#include <Updater.h>


#include <QApplication>
#include <QDebug>
#include <QTimer>

// static
QString UpdateChecker::EXECUTABLE_URL = "https://raw.githubusercontent.com/SSemenkin/AlarmBox/main/updates.json";
QString UpdateChecker::TRANSLATIONS_RU_URL = "https://raw.githubusercontent.com/SSemenkin/AlarmBox/main/updates_translations.json";


UpdateChecker::UpdateChecker(QObject *parent)
    : QObject{parent}
    , m_updater(QSimpleUpdater::getInstance())
    , m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &UpdateChecker::checkForUpdates);
    connect(m_updater, &QSimpleUpdater::checkingFinished, this, &UpdateChecker::checkForUpdatesFinished);
    connect(m_updater, &QSimpleUpdater::downloadFinished, this, &UpdateChecker::downloadFinished);
    setupUpdater();
}

void UpdateChecker::startChecking()
{
    if (m_timer->isActive()) {
        qDebug() << "UpdateChecker already checking updates.";
        return;
    }

    checkForUpdates();

    m_timer->start(1000 * 60 * 60); // every hour
}

void UpdateChecker::stopChecking()
{
    if (m_timer->isActive()) {
        m_timer->stop();
    }
}

bool UpdateChecker::isCheckingForUpdates() const
{
    return m_timer->isActive();
}

void UpdateChecker::update()
{
    m_updater->setDownloaderEnabled(TRANSLATIONS_RU_URL, true);
    m_updater->setDownloaderEnabled(EXECUTABLE_URL, true);

    m_updater->getUpdater(TRANSLATIONS_RU_URL)->manualUpdate();
    m_updater->getUpdater(EXECUTABLE_URL)->manualUpdate();
}

void UpdateChecker::checkForUpdates()
{
    m_updater->checkForUpdates(TRANSLATIONS_RU_URL);
}

void UpdateChecker::checkForUpdatesFinished(const QString &url)
{
    if (url == EXECUTABLE_URL) {
        emit updateAvaliable(m_updater->getUpdateAvailable(url));
        return;
    }

    m_updater->checkForUpdates(EXECUTABLE_URL);
}

void UpdateChecker::setupUpdater()
{
    m_updater->setModuleName(TRANSLATIONS_RU_URL, "translations");
    m_updater->setModuleVersion(TRANSLATIONS_RU_URL, APPLICATION_VERSION);
    m_updater->setNotifyOnUpdate(TRANSLATIONS_RU_URL, false);
    m_updater->setNotifyOnFinish(TRANSLATIONS_RU_URL, false);
    m_updater->setDownloaderEnabled(TRANSLATIONS_RU_URL, false);
    m_updater->setUseCustomInstallProcedures(TRANSLATIONS_RU_URL, true); // не открывать файл после завершения установки
    m_updater->setDownloadDirectory(TRANSLATIONS_RU_URL, qApp->applicationDirPath() + "/translations/");

    // executable file
    m_updater->setModuleVersion(EXECUTABLE_URL, APPLICATION_VERSION);
    m_updater->setNotifyOnFinish(EXECUTABLE_URL, false);
    m_updater->setNotifyOnUpdate(EXECUTABLE_URL, false);
    m_updater->setDownloaderEnabled(EXECUTABLE_URL, false);
    m_updater->setMandatoryUpdate(EXECUTABLE_URL, true);
}

void UpdateChecker::downloadFinished(const QString &url, const QString &filepath)
{
    Q_UNUSED(filepath);
    if (url == EXECUTABLE_URL) {
        Settings::instance()->setLastUpdates(m_updater->getChangelog(url));
    }
}
