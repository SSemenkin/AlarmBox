QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addcontrollerdialog.cpp \
    alarmdisplaywidget.cpp \
    alarminterrogator.cpp \
    alarmtreewidget.cpp \
    controllereditwidget.cpp \
    controllerlistwidget.cpp \
    controllerownership.cpp \
    controllersedit.cpp \
    editcontrollerdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    network/QTelnet.cpp \
    network/telnet.cpp \
    settings.cpp \
    settingsdialog.cpp

HEADERS += \
    addcontrollerdialog.h \
    alarmdisplaywidget.h \
    alarminterrogator.h \
    alarmtreewidget.h \
    controllereditwidget.h \
    controllerlistwidget.h \
    controllerownership.h \
    controllersedit.h \
    editcontrollerdialog.h \
    mainwindow.h \
    network/QTelnet.h \
    network/telnet.h \
    settings.h \
    settingsdialog.h

FORMS += \
    addcontrollerdialog.ui \
    alarmdisplaywidget.ui \
    controllereditwidget.ui \
    controllersedit.ui \
    editcontrollerdialog.ui \
    mainwindow.ui \
    settingsdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
