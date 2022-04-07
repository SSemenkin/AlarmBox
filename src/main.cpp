#include "mainwindow.h"
#include "version.h"

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QStyleFactory>


int main(int argc, char *argv[])
{
    static int count = 0;
    QApplication a(argc, argv);
    a.setApplicationVersion(APPLICATION_VERSION);
    a.setApplicationDisplayName(a.applicationName() +"v"+ APPLICATION_VERSION);
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
