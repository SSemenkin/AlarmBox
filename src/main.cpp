#include "mainwindow.h"
#include "version.h"
#include "customapplication.h"

#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QStyleFactory>
#include <QDir>
#include <QMessageBox>

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>

QFile logFile("alarms.log");
void (*defaultMessageHalder)(QtMsgType, const QMessageLogContext &, const QString &) = nullptr;

struct Update
{
    bool m_mandatory {true};
    QString m_changelog {"changelog"};
    QString m_downloadUrl;
    QString m_version;
    QString m_openUrl;

    QVariantMap toVariantMap() const {
        return QVariantMap {std::make_pair("open-url", m_openUrl),
                            std::make_pair("latest-version", m_version),
                            std::make_pair("download-url", m_downloadUrl),
                            std::make_pair("changelog", m_changelog),
                            std::make_pair("mandatory", m_mandatory)};
    }
};

void removeOldExecutableFile()
{
    QDir dir = qApp->applicationDirPath();
    QString filename = dir.filePath(qApp->applicationName() + ".exe.bak");

    if (QFile::exists(filename)) {
        QFile::remove(filename);
    }
}

void makeJSonFile(const QString &changelog, const QString &downloadUrl,
                  const QString &version = APPLICATION_VERSION, const QString& openUrl = "", bool mandatory = true)
{
    QFile f("upd.json");
    f.open(QIODevice::WriteOnly);
    QJsonDocument document;
    Update u {mandatory, changelog, downloadUrl, version, openUrl};
    QJsonObject obj;
    QJsonObject platform;
    platform.insert("windows", QJsonValue::fromVariant(u.toVariantMap()));
    obj.insert("updates", platform);
    document.setObject(obj);
    f.write(document.toJson());
    f.close();
}

void writeMessageToFile(const QString &message)
{
    QTextStream stream(&logFile);
    stream << QDateTime::currentDateTime().toString("[dd.MM.yyyy hh:mm] ") << message << '\n';
}

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message) {
    if (!logFile.isOpen()) {
        logFile.open(QIODevice::WriteOnly | QIODevice::Append);
    }

    if (!logFile.isOpen()) {
        return;
    }

    type == QtMsgType::QtInfoMsg ? writeMessageToFile(message) : defaultMessageHalder(type, context, message);
}

int main(int argc, char *argv[])
{
#ifdef UPDATE
    makeJSonFile("v.1.1.1\n.-Добавлена возможность(нужно было сразу) переключения темы без перезапуска приложения."
                 "\n-Теперь приложение будет автоматически проверять наличие обновлений."
                 "\nv.1.2\n -Добавлен лог аварий (alarms.log)", QString("https://github.com/SSemenkin/AlarmBox/blob/main/releases/%1/AlarmBoX.exe?raw=true")
                 .arg(APPLICATION_VERSION), APPLICATION_VERSION);
#endif

    CustomApplication a(argc, argv);

    removeOldExecutableFile();
    defaultMessageHalder = qInstallMessageHandler(messageHandler);
    a.setApplicationVersion(APPLICATION_VERSION);
    a.setApplicationDisplayName(a.applicationName());
    a.setStyle(QStyleFactory::create("fusion"));
    QLocale locale = Settings::instance()->getLocale();
    QTranslator translator;
    QTranslator qtTranslator;

    if (locale != QLocale(QLocale::English)) {
        bool result = translator.load("translations/" + a.applicationName()+ "_" + locale.name());
        qDebug() << (result ? "AlarmBox Successfull translation" : "AlarmBox Failed Translation");
        a.installTranslator(&translator);

        result = qtTranslator.load("translations/qt_" + locale.name());
        qDebug() << (result ? "Qt Successfull translation" : "Qt Failed Translation");
        a.installTranslator(&qtTranslator);
    }

    a.setFont(Settings::instance()->getFont());

    MainWindow w;
    w.setGeometry(Settings::instance()->getWindowGeometry());
    w.show();
    return a.exec();
}
