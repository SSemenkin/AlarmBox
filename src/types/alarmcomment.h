#ifndef ALARMCOMMENT_H
#define ALARMCOMMENT_H

#include "def.h"

struct AlarmComment
{
    explicit AlarmComment() = default;

    explicit AlarmComment(const QString& object, const QString &controller,
                 const QString& alarmType, const QString &description,
                 const QDateTime& createAt = QDateTime::currentDateTime());

    QVariantMap toVariantMap() const;
    static AlarmComment fromVariantMap(const QVariantMap &d);

    bool operator == (const AlarmComment& other) const;
    bool operator !=(const AlarmComment& other) const;

    friend QDebug operator << (QDebug dbg, const AlarmComment &c) {
        dbg << c.m_object << c.m_alarmType << c.m_description;
        return dbg;
    }

    QString m_object;
    QString m_controller;
    QString m_alarmType;
    QString m_description;
    QDateTime m_createAt;
};

#endif // ALARMCOMMENT_H
