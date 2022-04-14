#include "controllerownership.h"

ControllerOwnership *ControllerOwnership::m_instance = nullptr;

ControllerOwnership::ControllerOwnership(QObject *parent) : QObject(parent)
  , m_settings(*Settings::instance())
{
    Q_ASSERT_X(m_instance == nullptr, Q_FUNC_INFO, "Instance ControllerOwnership is already exists.");
    m_instance = this;
    loadControllersFromSettings();

}

ControllerOwnership::~ControllerOwnership()
{
    saveControllersToSettings();
}

ControllerOwnership *ControllerOwnership::instance()
{
    return m_instance;
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
    connect(controller.data(), &Telnet::loginStateChanged, this, &ControllerOwnership::processContollerAuthentication);
    controller->connectToNode();
    emit controllerAdded();
}

void ControllerOwnership::removeController(int index)
{
    Q_ASSERT(index >=0 && index < m_controllerList.size() && "Out of range");\
    Telnet *t = m_controllerList.at(index).data();
    emit controllerRemoved(t);
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
    Q_ASSERT(index >=0 && index < m_controllerList.size() && "Out of range");
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

const QList<QSharedPointer<Telnet>>& ControllerOwnership::controllerList() const
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
            m_controllerList.at(i)->reconnect();
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
    qDebug() << Q_FUNC_INFO << "Controller with hostname" << hostname << "not found";
}

void ControllerOwnership::loadControllersFromSettings()
{
    QList<ControllerInfo> infoList = m_settings.getControllersInfos();
    for (int i = 0; i < infoList.size(); ++i) {
        const ControllerInfo& info = infoList.at(i);
        addController(info.m_hostname,
                      info.m_username,
                      info.m_password);
    }
}

void ControllerOwnership::saveControllersToSettings()
{
    m_settings.setControllersInfos(m_controllerList);
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
