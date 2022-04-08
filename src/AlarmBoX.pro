QT       += core gui network

include($$PWD/../3rd_party/QSimpleUpdater/QSimpleUpdater.pri)
INCLUDEPATH += $$PWD/../3rd_party/QSimpleUpdater/src

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

win32* {
  RC_ICONS =$$PWD/icons/app.ico
}


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    customapplication.cpp \
    inheritancetreewidget.cpp \
    inheritanceview.cpp \
    main.cpp\
    addcontrollerdialog.cpp \
    addexceptiondialog.cpp \
    alarmdisplaywidget.cpp \
    alarminterrogator.cpp \
    alarmtreewidget.cpp \
    controllereditwidget.cpp \
    controllerlistwidget.cpp \
    controllerownership.cpp \
    controllersedit.cpp \
    editcontrollerdialog.cpp \
    exceptionspanel.cpp \
    mainwindow.cpp \
    network/QTelnet.cpp \
    network/telnet.cpp \
    rbslocation.cpp \
    settings.cpp \
    settingsdialog.cpp \
    types/alarm.cpp \
    types/alarmcomment.cpp \
    types/controllerinfo.cpp \
    types/displayexception.cpp

HEADERS += \
    addcontrollerdialog.h \
    addexceptiondialog.h \
    alarmdisplaywidget.h \
    alarminterrogator.h \
    alarmtreewidget.h \
    controllereditwidget.h \
    controllerlistwidget.h \
    controllerownership.h \
    controllersedit.h \
    customapplication.h \
    editcontrollerdialog.h \
    exceptionspanel.h \
    inheritancetreewidget.h \
    inheritanceview.h \
    mainwindow.h \
    network/QTelnet.h \
    network/telnet.h \
    rbslocation.h \
    settings.h \
    settingsdialog.h \
    types/alarm.h \
    types/alarmcomment.h \
    types/controllerinfo.h \
    types/def.h \
    types/displayexception.h \
    types/types.h \
    version.h

FORMS += \
    addcontrollerdialog.ui \
    addexceptiondialog.ui \
    alarmdisplaywidget.ui \
    controllereditwidget.ui \
    controllersedit.ui \
    editcontrollerdialog.ui \
    inheritanceview.ui \
    mainwindow.ui \
    settingsdialog.ui

TRANSLATIONS += \
        $$PWD/translations/AlarmBoX_ru.ts


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

OTHER_FILES += $$PWD/../updates.json\
               $$PWD/../updates_translations.json
