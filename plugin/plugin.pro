TARGET  = $$qtLibraryTarget(qrfilter)

TEMPLATE = lib
CONFIG += plugin link_pkgconfig
PKGCONFIG += QZXing

QT -= gui
QT += qml multimedia concurrent

target.path = $$[QT_INSTALL_QML]/ru/omprussia/qrfilter
qmldir.files += qmldir
qmldir.path +=  $$target.path

SOURCES += \
        plugin.cpp \
        qrfilter.cpp

HEADERS += \
        qrfilter.h

INSTALLS += target qmldir
