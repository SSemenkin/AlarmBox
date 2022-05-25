SOURCES += \
    $$PWD/maps/custommapgraphicsview.cpp \
    $$PWD/maps/RbsObject.cpp \
    $$PWD/maps/cell.cpp \
    $$PWD/nodeinfomodel.cpp \
    $$PWD/nodestatemodel.cpp \
    $$PWD/nodestateview.cpp \
    customapplication.cpp \
    inheritancetreewidget.cpp \
    inheritanceview.cpp \
    main.cpp\
    addcontrollerdialog.cpp \
    addexceptiondialog.cpp \
    addnodedialog.cpp \
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
    maps/mapwidget.cpp \
    maps/qsqliteworker.cpp\
    network/QTelnet.cpp \
    network/telnet.cpp \
    rbslocation.cpp \
    settings.cpp \
    settingsdialog.cpp \
    types/alarm.cpp \
    types/alarmcomment.cpp \
    types/controllerinfo.cpp \
    types/displayexception.cpp \
    updatechecker.cpp \
    types/node.cpp \
    processholder.cpp

HEADERS += \
    $$PWD/maps/custommapgraphicsview.h \
    $$PWD/maps/RbsObject.h \
    $$PWD/maps/TrueMatrix.h \
    $$PWD/maps/cell.h \
    $$PWD/nodeinfomodel.h \
    $$PWD/nodestatemodel.h \
    $$PWD/nodestateview.h \
    addcontrollerdialog.h \
    addexceptiondialog.h \
    addnodedialog.h \
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
    maps/mapwidget.h \
    maps/qsqliteworker.h\
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
    updatechecker.h \
    processholder.h \
    types/node.h \
    version.h

FORMS += \
    addcontrollerdialog.ui \
    addexceptiondialog.ui \
    addnodedialog.ui \
    alarmdisplaywidget.ui \
    controllereditwidget.ui \
    controllersedit.ui \
    editcontrollerdialog.ui \
    inheritanceview.ui \
    mainwindow.ui \
    settingsdialog.ui

TRANSLATIONS += \
        $$PWD/translations/AlarmBoX_ru.ts

RESOURCES += \
    r.qrc

OTHER_FILES += $$PWD/../updates.json\
               $$PWD/../updates_translations.json
