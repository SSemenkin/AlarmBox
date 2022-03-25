#include "alarmcomment.h"

AlarmComment::AlarmComment(const QString &object, const QString &controller, const QString &alarmType, const QString &description, const QDateTime &createAt)
    :   m_object(object)
    , m_controller(controller)
    , m_alarmType(alarmType)
    , m_description(description)
    , m_createAt(createAt)
{

}

QVariantMap AlarmComment::toVariantMap() const
{
    using std::make_pair;

    return QVariantMap({make_pair("object", m_object),
                        make_pair("alarmType", m_alarmType),
                        make_pair("description", m_description),
                        make_pair("controller", m_controller),
                        make_pair("createAt", m_createAt)});
}

//static
AlarmComment AlarmComment::fromVariantMap(const QVariantMap &d)
{
    QString object      = d["object"].toString();
    QString alarmType   = d["alarmType"].toString();
    QString description = d["description"].toString();
    QString controller  = d["controller"].toString();
    QDateTime createAt  = d["createAt"].toDateTime();
    return AlarmComment(object, controller, alarmType, description, createAt);
}

bool AlarmComment::operator ==(const AlarmComment &other) const
{
    return m_object == other.m_object &&
           m_alarmType == other.m_alarmType &&
           m_controller == other.m_controller &&
           m_description == other.m_description;
}

bool AlarmComment::operator !=(const AlarmComment &other) const
{
    return !operator==(other);
}
