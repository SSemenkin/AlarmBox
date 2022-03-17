#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(QStyleFactory::create("fusion"));
    QLocale locale = Settings::instance()->locale();
    QTranslator translator;
    QTranslator qtTranslator;

    if (locale != QLocale(QLocale::English)) {
        bool result = translator.load("translations/" + a.applicationName()+ "_" + locale.name());
        qDebug() << (result ? "Successfull translation" : "Failed Translation");
        a.installTranslator(&translator);

        qtTranslator.load("qt_" + QLocale::system().name(),
                    QLibraryInfo::location(QLibraryInfo::TranslationsPath));
        a.installTranslator(&qtTranslator);
    }
    MainWindow w;
    w.show();
    return a.exec();
}
