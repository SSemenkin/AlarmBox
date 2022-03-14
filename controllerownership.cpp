#include "controllerownership.h"

ControllerOwnership::ControllerOwnership(QObject *parent) : QObject(parent)
{
    loadControllersFromSettings();
}

ControllerOwnership::~ControllerOwnership()
{
    saveControllersToSettings();
}

void ControllerOwnership::addController(const QString &hostname, const QString &username, const QString &password)
{
    for (int i = 0; i < m_controllerList.size(); ++i) {
        if (m_controllerList.at(i)->hostname() == hostname) {
            return;
        }
    }
    QSharedPointer<Telnet> controller(new Telnet("nodeTitle", hostname, username, password));
    m_controllerList.push_back(controller);
    connect(controller.data(), &Telnet::loginState, this, &ControllerOwnership::processContollerAuthentication);
    controller->connectToNode();
}

void ControllerOwnership::removeController(int index)
{
    Q_ASSERT(index >=0 && index < m_controllerList.size());
    m_controllerList.removeAt(index);
}

void ControllerOwnership::removeController(const QString &hostname)
{
    for (int i = 0; i < m_controllerList.size(); ++i) {
        if (m_controllerList.at(i)->hostname() == hostname) {
            removeController(i);
        }
    }
}

QSharedPointer<Telnet> ControllerOwnership::controller(int index) const
{
    Q_ASSERT(index >=0 && index < m_controllerList.size());
    return m_controllerList.at(index);
}

QSharedPointer<Telnet> ControllerOwnership::controller(const QString &hostname) const
{
    for (int i = 0; i < m_controllerList.size(); ++i) {
        if (m_controllerList.at(i)->hostname() == hostname) {
            return m_controllerList.at(i);
        }
    }
    qDebug() << QString("WARNING:: controller with hostname %1 does not exists.").arg(hostname);
    return QSharedPointer<Telnet>(nullptr);
}

QList<QSharedPointer<Telnet>> ControllerOwnership::controllerList() const
{
    return m_controllerList;
}

void ControllerOwnership::reconnect(Telnet *telnet)
{
    telnet->connectToNode();
}

void ControllerOwnership::reconnect(const QString &hostname)
{
    for (int i = 0; i < m_controllerList.size(); ++i) {
        if (m_controllerList.at(i)->hostname() == hostname) {
            m_controllerList.at(i)->connectToNode();
            return;
        }
    }
}

void ControllerOwnership::onDetailControllerRequested(const QString &hostname)
{
    for (int i = 0; i < m_controllerList.size(); ++i) {
        if (m_controllerList.at(i)->hostname() == hostname) {
            emit controllerInfo(m_controllerList.at(i));
            return;
        }
    }
}

void ControllerOwnership::loadControllersFromSettings()
{
    QList<ControllerInfo> infoList = m_settings.getControllersInfos();
    for (int i = 0; i < infoList.size(); ++i) {
        const ControllerInfo& info = infoList.at(i);
        addController(info.m_hostname,
                      info.m_username,
                      info.m_password);
        qDebug() << info;
    }
}

void ControllerOwnership::saveControllersToSettings()
{
    m_settings.saveControllersInfos(m_controllerList);
}

void ControllerOwnership::processContollerAuthentication(bool state)
{
    Telnet *d = static_cast<Telnet*>(sender());
    if (!state) {
        emit controllerAuthenticationFailed(d);
    } else {
        emit controllerAuthenticationSuccessfull(d);
    }
}
