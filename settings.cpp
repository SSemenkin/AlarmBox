#include "settings.h"
#include "network/telnet.h"
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QRect>
#include <QFont>

#include "alarminterrogator.h"

Settings::Settings(QObject *parent)
    : QSettings(QStandardPaths::writableLocation(QStandardPaths::StandardLocation::AppConfigLocation) +"/AlarmBox/config.ini",
                QSettings::IniFormat, parent)
{

}

template <typename T>
void Settings::serialize(const T &data, const QString& filename) const
{
    QJsonDocument document;
    QJsonArray array;

    QFile f(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/" + filename);
    f.open(QIODevice::WriteOnly | QIODevice::Truncate);
    for (auto it = data.begin(); it != data.end(); ++it) {
        for (auto jt = it.value().begin(); jt != it.value().end(); ++jt) {
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

QLocale Settings::getLocale() const
{
    return value("language", QLocale(QLocale::English)).toLocale();
}

void Settings::setLocale(const QLocale &locale)
{
    setValue("language", locale);
}

bool Settings::getIsAutoRefreshEnabled() const
{
    return value("isAuto", true).toBool();
}

void Settings::setAutoRefreshEnabled(bool state)
{
    setValue("isAuto", state);
}

uint32_t Settings::getRefreshPeriod() const
{
    return value("period", 3).toUInt();
}

void Settings::setRefreshPeriod(uint32_t period)
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

void Settings::setWindowGeometry(const QRect &windowRect)
{
    setValue("window_geometry", windowRect);
}

QRect Settings::getWindowGeometry() const
{
    return value("window_geometry", QRect(0, 0, 800, 600)).toRect();
}

void Settings::setSplitterSizes(const QList<int> &sizes)
{
    QStringList s;
    for (int i = 0; i < sizes.size(); ++i) {
        s.push_back(QString::number(sizes[i]));
    }
    setValue("splitter_sizes", s);
}

QList<int> Settings::getSplitterSizes() const
{
    QStringList s =  value("splitter_sizes").toStringList();

    QList<int> result;
    for (int i = 0; i < s.size(); ++i) {
        result.push_back(s[i].toInt());
    }

    return result;
}

void Settings::setFont(const QFont &font)
{
    setValue("app_font_family", font.family());
    setValue("font_size", font.pointSize());
}

QFont Settings::getFont() const
{
   QFont result;
   result.setFamily(value("app_font_family").toString());
   result.setPointSize(value("font_size").toInt());
   return result;
}

QString Settings::getLocationFilepath() const
{
    return value("location_filename", "adresses.txt").toString();
}

void Settings::setExistingAlarms(const QVector<Alarm> &alarms)
{
    QJsonDocument document;
    QJsonArray array;

    QFile f(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/existing_alarms.json");
    f.open(QIODevice::WriteOnly | QIODevice::Truncate);
    for (int i = 0; i < alarms.size(); ++i) {
        array.push_back(QJsonValue::fromVariant(alarms.at(i).toVariantMap()));
    }
    document.setArray(array);
    f.write(document.toJson());
    f.close();
}

QVector<Alarm> Settings::getExistingAlarms() const
{

    QFile f(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/existing_alarms.json");
    f.open(QIODevice::ReadOnly);

    QJsonDocument document = QJsonDocument::fromJson(f.readAll());
    QJsonArray array = document.array();
    QVector<Alarm> result;

    for (int i = 0; i < array.size(); ++i) {
         QVariantMap m = array[i].toObject().toVariantMap();
         result.push_back(Alarm::fromVariantMap(m));
    }

    f.close();
    return result;
}

void Settings::setLocationFilepath(const QString &filepath)
{
    setValue("location_filename", filepath);
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
