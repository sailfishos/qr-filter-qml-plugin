TARGET  = $$qtLibraryTarget(qrfilter)

TEMPLATE = lib
CONFIG += plugin

QT = core dbus qml multimedia

target.path = $$[QT_INSTALL_QML]/Amber/QrFilter
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
