#include "settings.h"
#include "network/telnet.h"
#include <QStandardPaths>


Settings::Settings(QObject *parent)
    : QSettings(QStandardPaths::writableLocation(QStandardPaths::StandardLocation::AppConfigLocation) + "/config.ini",
                QSettings::IniFormat, parent)
{

}

void Settings::saveControllersInfos(const QList<QSharedPointer<Telnet>> &controllersList)
{
    clear();
    if (controllersList.isEmpty()) {
        return;
    }

    setValue(decodeEncodeData("controllers_count"), controllersList.size());
    for (int i = 0; i < controllersList.size(); ++i) {
        setValue(decodeEncodeData(QString("hostname_%1").arg(i)), decodeEncodeData(controllersList.at(i)->hostname()));
        setValue(decodeEncodeData(QString("username_%1").arg(i)), decodeEncodeData(controllersList.at(i)->username()));
        setValue(decodeEncodeData(QString("password_%1").arg(i)), decodeEncodeData(controllersList.at(i)->password()));
    }
}

QList<ControllerInfo> Settings::getControllersInfos() const
{
    QList<ControllerInfo> result;

    int count = value(decodeEncodeData("controllers_count"), 0).toInt();
    ControllerInfo temporary;
    for (int i = 0; i < count; ++i) {
        temporary.m_hostname = decodeEncodeData(value(decodeEncodeData(QString("hostname_%1").arg(i))).toString());
        temporary.m_username = decodeEncodeData(value(decodeEncodeData(QString("username_%1").arg(i))).toString());
        temporary.m_password = decodeEncodeData(value(decodeEncodeData(QString("password_%1").arg(i))).toString());
        result.push_back(temporary);
    }

    return result;
}

QString Settings::decodeEncodeData(const QString &input, const QString key)
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
