#ifndef DISPLAYEXCEPTION_H
#define DISPLAYEXCEPTION_H

#include "def.h"

struct DisplayException
{
    explicit DisplayException() = default;
    explicit DisplayException(const QString &object, const QString &alarmType, const QString &controller);

    explicit DisplayException(const QString &object, int alarmType, const QString &controller);


    QVariantMap toVariantMap() const;
    static DisplayException fromVariantMap(const QVariantMap &d);

    bool operator == (const DisplayException& other) const;

    QString m_object;
    QString m_alarmType;
    QString m_controller;

    friend QDebug operator << (QDebug debug, const DisplayException &a) {
        debug << "DisplayException(" <<a.m_controller << ", " <<
                 a.m_object << ", " << a.m_alarmType << ")";
        return debug;
    }
};


#endif // DISPLAYEXCEPTION_H
