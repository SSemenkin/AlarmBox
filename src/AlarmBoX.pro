QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
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
    editcontrollerdialog.h \
    exceptionspanel.h \
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
    types/types.h

FORMS += \
    addcontrollerdialog.ui \
    addexceptiondialog.ui \
    alarmdisplaywidget.ui \
    controllereditwidget.ui \
    controllersedit.ui \
    editcontrollerdialog.ui \
    mainwindow.ui \
    settingsdialog.ui

TRANSLATIONS += \
        ../translations/AlarmBox_ru.ts


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
