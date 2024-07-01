HEADERS += \
    $$PWD/GlobalDef.h \
    $$PWD/MainWindow.h

SOURCES += \
    $$PWD/MainWindow.cpp \
    $$PWD/main.cpp

FORMS += \
    $$PWD/MainWindow.ui

INCLUDEPATH += $$PWD/tabbasic
include($$PWD/tabbasic/tabbasic.pri)
INCLUDEPATH += $$PWD/tabdraw
include($$PWD/tabdraw/tabdraw.pri)
INCLUDEPATH += $$PWD/tab3d
include($$PWD/tab3d/tab3d.pri)
INCLUDEPATH += $$PWD/tabchart
include($$PWD/tabchart/tabchart.pri)
