TARGET  = $$qtLibraryTarget(qrfilter)

TEMPLATE = lib
CONFIG += plugin

QT = core dbus qml multimedia
LIBS += -lrt

target.path = $$[QT_INSTALL_QML]/ru/omprussia/qrfilter
qmldir.files += qmldir
qmldir.path +=  $$target.path

SOURCES += \
        plugin.cpp \
        qrfilter.cpp \
        videofilterrunnable.cpp

HEADERS += \
        qrfilter.h \
        videofilterrunnable.h

INSTALLS += target qmldir
