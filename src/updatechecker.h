#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QObject>

class QSimpleUpdater;
class QTimer;

class UpdateChecker : public QObject
{
    Q_OBJECT
public:
    explicit UpdateChecker(QObject *parent = nullptr);
    void startChecking();
    void stopChecking();
    bool isCheckingForUpdates() const;
    void update();
signals:
    void updateAvaliable(bool state);
private:
    void checkForUpdates();
    void checkForUpdatesFinished(const QString &url);
    void setupUpdater();
    void downloadFinished(const QString &url, const QString &filepath);
private:
    QSimpleUpdater *m_updater;
    QTimer  *m_timer;

    static QString EXECUTABLE_URL;
    static QString TRANSLATIONS_RU_URL;
};

#endif // UPDATECHECKER_H
