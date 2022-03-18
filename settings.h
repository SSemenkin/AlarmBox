#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QCoreApplication>
#include <QDebug>
#include <QDateTime>

#include "network/telnet.h"

struct ControllerInfo
{
    explicit ControllerInfo(QSharedPointer<Telnet> controller) :
        m_hostname(controller->hostname()),
        m_username(controller->username()),
        m_password(controller->password())
    {

    }
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

struct DisplayException
{
    explicit DisplayException(const QString &object,
                     const QString &alarmType,
                     const QString &controller) :
        m_object(object)
      , m_alarmType(alarmType)
      , m_controller(controller)
    {

    }
    explicit DisplayException() = default;

    QString m_object;
    QString m_alarmType;
    QString m_controller;
};

struct AlarmComment
{
    QString m_object;
    QString m_controller;
    QString m_alarmType;
    QString m_description;
    QDateTime m_createAt;

    AlarmComment() = default;

    AlarmComment(const AlarmComment&) = default;
    AlarmComment& operator=(const AlarmComment&) = default;

    AlarmComment(const QString& object, const QString &controller,
                 const QString& alarmType, const QString &description,
                 const QDateTime& createAt = QDateTime::currentDateTime()) :
        m_object(object)
      , m_controller(controller)
      , m_alarmType(alarmType)
      , m_description(description)
      , m_createAt(createAt)
    {

    }

    QVariantMap toVariantMap() const {
        return QVariantMap({std::make_pair("object", m_object),
                            std::make_pair("alarmType", m_alarmType),
                            std::make_pair("description", m_description),
                            std::make_pair("controller", m_controller),
                            std::make_pair("createAt", m_createAt)});
    }

    bool operator == (const AlarmComment& other) const {
        return m_object == other.m_object &&
               m_alarmType == other.m_alarmType &&
               m_controller == other.m_controller &&
               m_description == other.m_description;
    }

    bool operator !=(const AlarmComment& other) const {
        return !operator==(other);
    }

    friend QDebug operator << (QDebug dbg, const AlarmComment &c) {
        dbg << c.m_object << c.m_alarmType << c.m_description;
        return dbg;
    }
};

class Settings : protected QSettings
{
    Q_OBJECT
public:
    static Settings* instance();
    void setControllersInfos(const QList<QSharedPointer<Telnet>> &controllersList);
    QList<ControllerInfo> getControllersInfos();

    static QString decodeEncodeData(const QString &input, const QString &key = "%31_)*&z;");

    QLocale locale() const;
    void setLocale(const QLocale &locale);

    bool autoRefreshEnabled() const;
    void setRefreshEnabled(bool state);

    uint32_t period() const;
    void setPeriod(uint32_t period);

    void addException(const DisplayException &exception);
    QList<DisplayException> getExceptions() const;

    void setAlarmComments(const QMap<QString, QMap<QString, AlarmComment>> &controllerComments);
    QMap<QString, QMap<QString, AlarmComment>> getAlarmComments() const;

protected:
    explicit Settings(QObject *parent = nullptr);
};

#endif // SETTINGS_H
