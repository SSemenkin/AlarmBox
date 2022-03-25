#ifndef CONTROLLERINFO_H
#define CONTROLLERINFO_H

#include "def.h"

class Telnet;

struct ControllerInfo
{
    explicit ControllerInfo() = default;
    explicit ControllerInfo(QSharedPointer<Telnet> controller);

    QString m_hostname;
    QString m_username;
    QString m_password;

    friend QDebug operator << (QDebug debug, const ControllerInfo &obj) {
        debug << "ControllerInfo { " << obj.m_hostname << " ; " << obj.m_username << " ; " <<
                 obj.m_password << " }";
        return debug;
    }
};

#endif // CONTROLLERINFO_H
