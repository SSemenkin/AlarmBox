#ifndef CONTROLLEROWNERSHIP_H
#define CONTROLLEROWNERSHIP_H

#include <QList>
#include "network/telnet.h"
#include "settings.h"

class ControllerOwnership : public QObject
{
    Q_OBJECT
public:
    ControllerOwnership(QObject *parent = nullptr);
    virtual ~ControllerOwnership();

    void addController(const QString &hostname,
                       const QString &username,
                       const QString &password);

    void removeController(const QString &hostname);

    QSharedPointer<Telnet> controller(int index) const;
    QSharedPointer<Telnet> controller(const QString &hostname) const;
    const QList<QSharedPointer<Telnet>>& controllerList() const;


    void reconnect(Telnet *telnet);
    void reconnect(const QString &hostname);

    void onDetailControllerRequested(const QString &hostname);

signals:
    void controllerAdded();
    void controllerRemoved();
    void controllerAuthenticationFailed(Telnet *conroller);
    void controllerAuthenticationSuccessfull(Telnet *controller);
    void controllerInfo(QSharedPointer<Telnet> controller);

private:
    void loadControllersFromSettings();
    void saveControllersToSettings();
    void processContollerAuthentication(bool state);
    void removeController(int index);

private:
    QList<QSharedPointer<Telnet>> m_controllerList;
    Settings m_settings;
};

#endif // CONTROLLEROWNERSHIP_H
