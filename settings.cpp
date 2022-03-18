#include "settings.h"
#include "network/telnet.h"
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>


Settings::Settings(QObject *parent)
    : QSettings(QStandardPaths::writableLocation(QStandardPaths::StandardLocation::AppConfigLocation) +"/AlarmBox/config.ini",
                QSettings::IniFormat, parent)
{

}

template <typename T>
void Settings::serialize(const QMap<QString, QMap<QString, T>> &data, const QString& filename) const
{
    QJsonDocument document;
    QJsonArray array;

    QFile f(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/" + filename);
    f.open(QIODevice::WriteOnly | QIODevice::Truncate);
    for (auto it = data.begin(); it != data.end(); ++it) {
        for (auto jt = it.value().begin(); jt != it.value().end(); ++jt) {
            QJsonValue value;
            array.push_back(QJsonValue::fromVariant((*jt).toVariantMap()));
        }
    }
    document.setArray(array);
    f.write(document.toJson());
    f.close();
}

template <typename T>
void Settings::serialize(const QMap<QString, QVector<T>> &data, const QString& filename) const
{
    QJsonDocument document;
    QJsonArray array;

    QFile f(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/" + filename);
    f.open(QIODevice::WriteOnly | QIODevice::Truncate);
    for (auto it = data.begin(); it != data.end(); ++it) {
        for (auto jt = it.value().begin(); jt != it.value().end(); ++jt) {
            QJsonValue value;
            array.push_back(QJsonValue::fromVariant((*jt).toVariantMap()));
        }
    }
    document.setArray(array);
    f.write(document.toJson());
    f.close();
}

Settings* Settings::instance()
{
    static Settings s;
    return &s;
}

void Settings::setControllersInfos(const QList<QSharedPointer<Telnet>> &controllersList)
{

    if (controllersList.isEmpty()) {
        return;
    }

    beginGroup(decodeEncodeData("Controllers"));
    remove("");

    setValue(decodeEncodeData("controllers_count"), controllersList.size());

    for (int i = 0; i < controllersList.size(); ++i) {
        setValue(decodeEncodeData(QString("hostname_%1").arg(i)), decodeEncodeData(controllersList.at(i)->hostname()));
        setValue(decodeEncodeData(QString("username_%1").arg(i)), decodeEncodeData(controllersList.at(i)->username()));
        setValue(decodeEncodeData(QString("password_%1").arg(i)), decodeEncodeData(controllersList.at(i)->password()));
    }
    endGroup();
}

QList<ControllerInfo> Settings::getControllersInfos()
{
    QList<ControllerInfo> result;

    beginGroup(decodeEncodeData("Controllers"));
    int count = value(decodeEncodeData("controllers_count"), 0).toInt();

    ControllerInfo temporary;
    for (int i = 0; i < count; ++i) {
        temporary.m_hostname = decodeEncodeData(value(decodeEncodeData(QString("hostname_%1").arg(i))).toString());
        temporary.m_username = decodeEncodeData(value(decodeEncodeData(QString("username_%1").arg(i))).toString());
        temporary.m_password = decodeEncodeData(value(decodeEncodeData(QString("password_%1").arg(i))).toString());
        result.push_back(temporary);
    }
    endGroup();

    return result;
}

QLocale Settings::locale() const
{
    return value("language", QLocale(QLocale::English)).toLocale();
}

void Settings::setLocale(const QLocale &locale)
{
    setValue("language", locale);
}

bool Settings::autoRefreshEnabled() const
{
    return value("isAuto", true).toBool();
}

void Settings::setRefreshEnabled(bool state)
{
    setValue("isAuto", state);
}

uint32_t Settings::period() const
{
    return value("period", 3).toUInt();
}

void Settings::setPeriod(uint32_t period)
{
    setValue("period", period);
}

void Settings::setAlarmComments(const QMap<QString, QMap<QString, AlarmComment>> &controllerComments)
{
    serialize(controllerComments, "comments.json");
}

QMap<QString, QMap<QString, AlarmComment>> Settings::getAlarmComments() const
{
    QFile f(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/comments.json");
    f.open(QIODevice::ReadOnly);

    QJsonDocument document = QJsonDocument::fromJson(f.readAll());
    QJsonArray array = document.array();
    QMap<QString, QMap<QString, AlarmComment>> result;

    for (int i = 0; i < array.size(); ++i) {
         QVariantMap m = array[i].toObject().toVariantMap();

         QString controller = m["controller"].toString();
         QString object = m["object"].toString();
         QString description = m["description"].toString();
         QString alarmType = m["alarmType"].toString();
         QDateTime createAt = m["createAt"].toDateTime();

         result[controller][object] = AlarmComment(object, controller, alarmType, description, createAt);
    }

    f.close();
    return result;
}

void Settings::setDisplayExceptions(const QMap<QString, QVector<DisplayException>> &exceptions)
{
    serialize(exceptions, "exceptions.json");
}

QMap<QString, QVector<DisplayException>> Settings::getExceptions() const
{
    QFile f(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/exceptions.json");
    f.open(QIODevice::ReadOnly);

    QJsonDocument document = QJsonDocument::fromJson(f.readAll());
    QJsonArray array = document.array();
    QMap<QString, QVector<DisplayException>> result;

    for (int i = 0; i < array.size(); ++i) {
         QVariantMap m = array[i].toObject().toVariantMap();

         QString controller = m["controller"].toString();
         QString object = m["object"].toString();
         QString alarmType = m["alarmType"].toString();

         result[controller].push_back(DisplayException(object, alarmType, controller));
    }

    f.close();
    return result;
}

QString Settings::decodeEncodeData(const QString &input, const QString &key)
{
    QByteArray inputData = input.toLatin1();
    const char *rawInput = inputData.data();
    int inputLength = inputData.length();

    QByteArray keyData = key.toLatin1();
    const char *rawKey = keyData.data();
    int keyLength = keyData.length();

    QString result;

    char r[inputLength];

    for (int i = 0; i < inputLength; ++i) {
        r[i] = rawInput[i] ^ rawKey[i % keyLength];
    }

    result = QString::fromLatin1(r, inputLength);
    return result;
}
