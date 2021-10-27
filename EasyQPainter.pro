QT += core gui widgets concurrent

CONFIG += c++11 utf8_source

DEFINES += QT_DEPRECATED_WARNINGS
win32{
RC_ICONS = $$PWD/img/icon.ico
}
DESTDIR = $$PWD/bin

INCLUDEPATH += $$PWD/src
include($$PWD/src/src.pri)

DISTFILES += \
    LICENSE \
    README.md

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    img/img.qrc

