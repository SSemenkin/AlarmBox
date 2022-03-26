#ifndef ALARM_H
#define ALARM_H

#include "def.h"

class Alarm
{
public:
    enum class State : int32_t {
        Invalid = -1,
        Normal,
        Raised,
        Cleared
    };
    enum class Category : int32_t {
        A1 = 0,
        A2,
        A3,
        A4
    };

    bool operator == (const Alarm& other) const;
    bool operator < (const Alarm& other) const;

    static QStringList alarmLabels();
    static QString descriptionFromCategory(Category category);


    QVariantMap toVariantMap() const;
    static Alarm fromVariantMap(const QVariantMap &d);

    QString   m_object;
    QString   m_description;
    QString   m_controller;
    QString   m_controllerTitle;
    QDateTime m_raisedTime;
    QDateTime m_clearedTime;
    State     m_state {State::Raised};
    Category  m_category;

private:
    static QString serializeDescription(Category category, const QString &existingDescription);
    static QString deserializeDescription(Category category, const QString &existingDescription = QString());
};

#endif // ALARM_H
