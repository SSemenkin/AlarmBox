#include "displayexception.h"

DisplayException::DisplayException(const QString &object, const QString &alarmType, const QString &controller) :
    m_object(object)
  , m_alarmType(alarmType)
  , m_controller(controller)
{

}

DisplayException::DisplayException(const QString &object, int alarmType, const QString &controller) :
    DisplayException(object, QString::number(alarmType), controller)
{

}

QVariantMap DisplayException::toVariantMap() const
{
    using std::make_pair;

    return QVariantMap({make_pair("controller", m_controller),
                        make_pair("object", m_object),
                        make_pair("alarmType", m_alarmType)});
}

//static
DisplayException DisplayException::fromVariantMap(const QVariantMap &d)
{
    QString controller = d["controller"].toString();
    QString object = d["object"].toString();
    QString alarmType = d["alarmType"].toString();
    return DisplayException(object, alarmType, controller);
}

bool DisplayException::operator ==(const DisplayException &other) const
{
    return m_object == other.m_object &&
           m_controller == other.m_controller &&
           m_alarmType == other.m_alarmType;
}
