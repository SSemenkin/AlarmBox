#include "mainwindow.h"
#include "version.h"
#include "customapplication.h"
#include "maps/qsqliteworker.h"

#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QStyleFactory>
#include <QDir>
#include <QMessageBox>

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>


#include <iostream>

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


void installTranslators(QTranslator *appTranlator, QTranslator *qtTranslator,
                        QApplication *app, const QLocale &locale)
{
    if (appTranlator->load("translations/" + app->applicationName() + "_" + locale.name())) {
        qDebug() <<  "AlarmBox successfull translation";
        app->installTranslator(appTranlator);
    } else {
        std::cerr << "AlarmBox failed translation\n";
    }

    if (qtTranslator->load("translations/qt_" + locale.name())) {
        qDebug() << "Qt successfull translation";
        app->installTranslator(qtTranslator);
    } else {
        std::cerr << "Qt failed translation\n";
    }
}

//#define UPDATE_OBJECTS_DB

struct NodeCell {
    QString nodename;
    QString cellid;
    float angle;

    bool operator == (const NodeCell& other) const {
        return nodename == other.nodename &&
                cellid == other.cellid &&
               angle == other.angle;
    }
};
struct MapNode {
    QString name;
    QPointF position;
    bool operator == (const MapNode& other) const {
        return name == other.name && position == other.position;
    }
};

struct NodeInfo {
    QVector<NodeCell> cells;
    QVector<MapNode> nodes;

    void addCell(const NodeCell &cell) {
        if (!cells.contains(cell)) {
            cells.push_back(cell);
        }
    }

    void addNode(const MapNode &node) {
        if (!nodes.contains(node)) {
            nodes.push_back(node);
        }
    }
};

QString transform(QString source) {
    return source.replace(',', '.').remove('N').remove('E').left(8);
}
QString loadStylesheet(const QString &filepath)
{
    QFile f(filepath);
    if (f.open(QIODevice::ReadOnly)) {
        return f.readAll();
    }
    return QString();
}
NodeInfo loadFromFile(const QString &filepath)
{
    NodeInfo result;

    QFile f (filepath);
    if (f.open(QIODevice::ReadOnly)) {
        QStringList d = QString(f.readAll()).split("\r\n");
        for (int i = 1; i < d.size(); ++i) {
            QStringList row = d.at(i).split(';', Qt::SkipEmptyParts);

            if (row.isEmpty()) continue;

            result.addCell(NodeCell{row.at(0), row.at(1), row.at(2).toFloat()});
            result.addNode(MapNode{row.at(0), QPointF {transform(row[3]).toFloat(), transform(row[4]).toFloat()}});
        }
    } else qDebug() << f.errorString();
    return result;
}

int main(int argc, char *argv[])
{

#ifdef UPDATE
    makeJSonFile("v.1.1.1\n.-Добавлена возможность(нужно было сразу) переключения темы без перезапуска приложения."
                 "\n-Теперь приложение будет автоматически проверять наличие обновлений."
                 "\nv.1.2\n -Добавлен лог аварий (alarms.log)", QString("https://github.com/SSemenkin/AlarmBox/blob/main/releases/%1/AlarmBoX.exe?raw=true")
                 .arg(APPLICATION_VERSION), APPLICATION_VERSION);
#endif

#ifdef UPDATE_OBJECTS_DB
    QSqliteWorker worker;
    NodeInfo info = loadFromFile(":/rbs_data.csv");
    for (const Node &node : info.nodes) {
        worker.addRbs(node.name, node.position, "");
    }

    for (const NodeCell &cell : info.cells) {
        worker.addCell(cell.nodename, cell.cellid, cell.angle);
    }
#endif

    CustomApplication a(argc, argv);
    a.setApplicationName("AlarmBox");

    removeOldExecutableFile();
    defaultMessageHalder = qInstallMessageHandler(messageHandler);
    a.setApplicationVersion(APPLICATION_VERSION);
    a.setApplicationDisplayName(a.applicationName());
    a.setStyle(QStyleFactory::create("fusion"));

    QLocale locale = Settings::instance()->getLocale();
    QScopedPointer<QTranslator> translator;
    QScopedPointer<QTranslator> qtTranslator;

    if (locale != QLocale(QLocale::English)) {
        translator.reset(new QTranslator);
        qtTranslator.reset(new QTranslator);
        installTranslators(translator.data(), qtTranslator.data(), &a, locale);
    }

    a.setFont(Settings::instance()->getFont());

    MainWindow w;
    w.setGeometry(Settings::instance()->getWindowGeometry());
    w.show();
    return a.exec();
}
