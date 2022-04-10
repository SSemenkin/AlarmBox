#include "mainwindow.h"
#include "version.h"
#include "customapplication.h"

#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QStyleFactory>
#include <QDir>
#include <QMessageBox>

//TODO сделать логгер сообщений.

void removeOldExecutableFile()
{
    QDir dir = qApp->applicationDirPath();
    QString filename = dir.filePath(qApp->applicationName() + ".exe.bak");

    if (QFile::exists(filename)) {
        QFile::remove(filename);
    }
}

int main(int argc, char *argv[])
{
    CustomApplication a(argc, argv);
    removeOldExecutableFile();
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
