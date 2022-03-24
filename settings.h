#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QCoreApplication>
#include <QDebug>
#include <QDateTime>
#include "network/telnet.h"

struct Alarm;

struct ControllerInfo
{
    explicit ControllerInfo() = default;
    explicit ControllerInfo(QSharedPointer<Telnet> controller) :
        m_hostname(controller->hostname()),
        m_username(controller->username()),
        m_password(controller->password())
    {

    }

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
    explicit DisplayException() = default;
    explicit DisplayException(const QString &object,
                     const QString &alarmType,
                     const QString &controller) :
        m_object(object)
      , m_alarmType(alarmType)
      , m_controller(controller)
    {

    }
    explicit DisplayException(const QString &object, int alarmType, const QString &controller) :
        DisplayException(object, QString::number(alarmType), controller) {}



    QVariantMap toVariantMap() const {
        return QVariantMap({std::make_pair("controller", m_controller),
                            std::make_pair("object", m_object),
                            std::make_pair("alarmType", m_alarmType)});
    }

    friend QDebug operator << (QDebug debug, const DisplayException &a) {
        debug << "DisplayException(" <<a.m_controller << ", " <<
                 a.m_object << ", " << a.m_alarmType << ")";
        return debug;
    }

    bool operator == (const DisplayException& other) const {
        return m_object == other.m_object &&
               m_controller == other.m_controller &&
               m_alarmType == other.m_alarmType;
    }

    QString m_object;
    QString m_alarmType;
    QString m_controller;
};

struct AlarmComment
{
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

    QString m_object;
    QString m_controller;
    QString m_alarmType;
    QString m_description;
    QDateTime m_createAt;
};


class Settings : protected QSettings // наследование protected для того чтобы закрыть доступ к методам базового класса
                                     // и оставить такую возможность только для наследников.
{
    Q_OBJECT
public:
    static Settings* instance();

    void setControllersInfos(const QList<QSharedPointer<Telnet>> &controllersList);
    QList<ControllerInfo> getControllersInfos();

    static QString decodeEncodeData(const QString &input, const QString &key = "%31_)*&z;");

    void setLocale(const QLocale &locale);
    QLocale getLocale() const;

    void setAutoRefreshEnabled(bool state);
    bool getIsAutoRefreshEnabled() const;

    void setRefreshPeriod(uint32_t period);
    uint32_t getRefreshPeriod() const;

    void setAlarmComments(const QMap<QString, QMap<QString, AlarmComment>> &controllerComments);
    QMap<QString, QMap<QString, AlarmComment>> getAlarmComments() const;

    void setDisplayExceptions(const QMap<QString, QVector<DisplayException>>& exceptions);
    QMap<QString, QVector<DisplayException>> getExceptions() const;

    void setWindowGeometry(const QRect &windowRect);
    QRect getWindowGeometry() const;

    void setSplitterSizes(const QList<int> &sizes);
    QList<int> getSplitterSizes() const;

    void setFont(const QFont &font);
    QFont getFont() const;

    void setLocationFilepath(const QString &filepath);
    QString getLocationFilepath() const;

    void setExistingAlarms(const QVector<Alarm> &alarms);
    QVector<Alarm> getExistingAlarms() const;

protected:
    explicit Settings(QObject *parent = nullptr);

private:
    ///@brief Метод сериализации данных в JSON формат.
    /// @param data контейнер данных
    /// накладывает на объект, такие требование что получаемый при итерации объект должен быть сам итерируем
    /// и внутри такого объекта необходимо реализовать метод QVariantMap toVariantMap()
    /// @param filename
    /// имя файла (будет распологаться в user/appdata/local/programm/filename.prefix
    template<typename T>
    void serialize(const T &data, const QString& filename) const;
};

#endif // SETTINGS_H
