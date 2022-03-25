#include "alarm.h"


bool Alarm::operator ==(const Alarm &other) const
{
    return m_object      == other.m_object &&
           m_description == other.m_description &&
           m_controller  == other.m_controller;
}

bool Alarm::operator<(const Alarm &other) const
{
    return m_object < other.m_object;
}

// static
QStringList Alarm::alarmLabels()
{
    static QStringList d {QObject::tr("Manually blocked"), QObject::tr("Halted"), QObject::tr("Not works")};
    return d;
}

QString Alarm::descriptionFromCategory(Category category)
{
    return category == Category::A1 ? QString() : alarmLabels().at(static_cast<int>(category) - 1);
}

// static
QString Alarm::deserializeDescription(Category category, const QString &existingDescription)
{
    return category == Category::A1 ? existingDescription : alarmLabels().at(static_cast<int>(category) - 1);
}

//static
QString Alarm::serializeDescription(Category category, const QString &existingDescription)
{
    return category == Category::A1 ? existingDescription : QString::number(static_cast<int>(category));
}

QVariantMap Alarm::toVariantMap() const
{
    using std::make_pair;

    return QVariantMap({make_pair("object", m_object),
                        make_pair("description", serializeDescription(m_category, m_description)),
                        make_pair("controller", m_controller),
                        make_pair("controllerTitle", m_controllerTitle),
                        make_pair("raisedTime", m_raisedTime),
                        make_pair("state", static_cast<int>(m_state)),
                        make_pair("category", static_cast<int>(m_category))});
}

//static
Alarm Alarm::fromVariantMap(const QVariantMap &d)
{
    Alarm r;
    r.m_object = d["object"].toString();
    r.m_category = static_cast<Category>(d["category"].toInt());
    r.m_description = deserializeDescription(r.m_category, d["description"].toString());
    r.m_controller = d["controller"].toString();
    r.m_controllerTitle = d["controllerTitle"].toString();
    r.m_raisedTime = d["raisedTime"].toDateTime();
    r.m_state = static_cast<State>(d["state"].toInt());
    return r;
}
