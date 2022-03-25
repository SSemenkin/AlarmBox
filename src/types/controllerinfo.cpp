#include "controllerinfo.h"
#include "network/telnet.h"

ControllerInfo::ControllerInfo(QSharedPointer<Telnet> controller) :
    m_hostname(controller->hostname())
  , m_username(controller->username())
  , m_password(controller->password())
{

}
