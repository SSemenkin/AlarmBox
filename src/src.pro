QT       += core gui network sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS_DEBUG += -g3 -pg
QMAKE_LFLAGS_DEBUG += -pg -lgmon
#### QSimpleUpdater library
include($$PWD/../3rd_party/QSimpleUpdater/QSimpleUpdater.pri)
INCLUDEPATH += $$PWD/../3rd_party/QSimpleUpdater/src

#### MapGraphics library
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../3rd_party/MapGraphics/MapGraphics/release/ -lMapGraphics
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../3rd_party/MapGraphics/MapGraphics/debug/ -lMapGraphics
else:unix: LIBS += -L$$OUT_PWD/../3rd_party/MapGraphics/MapGraphics/ -lMapGraphics

INCLUDEPATH += $$PWD/../3rd_party/MapGraphics/MapGraphics
DEPENDPATH += $$PWD/../3rd_party/MapGraphics/MapGraphics


CONFIG += c++14

win32* {
  RC_ICONS =$$PWD/icons/app.ico
}


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
include(src.pri)


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target






