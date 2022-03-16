#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QCoreApplication>
#include <QDebug>

#include "network/telnet.h"

struct ControllerInfo {

    explicit ControllerInfo(QSharedPointer<Telnet> controller) :
        m_hostname(controller->hostname()),
        m_username(controller->username()),
        m_password(controller->password())
    {

    };
    explicit ControllerInfo() = default;

    QString m_hostname;
    QString m_username;
    QString m_password;

    friend QDebug operator << (QDebug debug, const ControllerInfo &obj) {
        debug << "ControllerInfo { " << obj.m_hostname << " ; " << obj.m_username << " ; " <<
                 obj.m_password << " }";
        return debug;
    }
};

class Settings : public QSettings
{
    Q_OBJECT
public:
    static Settings* instance();
    void saveControllersInfos(const QList<QSharedPointer<Telnet>> &controllersList);
    QList<ControllerInfo> getControllersInfos() const;

    static QString decodeEncodeData(const QString &input, const QString key = "%31_)*&z;");

    QLocale locale() const;
    void setLocale(const QLocale &locale);

    bool autoRefreshEnabled() const;
    void setRefreshEnabled(bool state);

    uint32_t period() const;
    void setPeriod(uint32_t period);

protected:
    explicit Settings(QObject *parent = nullptr);
};

#endif // SETTINGS_H
