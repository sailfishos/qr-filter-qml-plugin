TARGET  = $$qtLibraryTarget(qrfilter)

TEMPLATE = lib
CONFIG += plugin

QT = core dbus qml multimedia

target.path = $$[QT_INSTALL_QML]/Amber/QrFilter
qmldir.files += qmldir plugins.qmltypes
qmldir.path +=  $$target.path

SOURCES += \
        plugin.cpp \
        qrfilter.cpp \
        videofilterrunnable.cpp

HEADERS += \
        qrfilter.h \
        videofilterrunnable.h

qmltypes.commands = qmlplugindump -nonrelocatable Amber.QrFilter 1.0 > $$PWD/plugins.qmltypes

QMAKE_EXTRA_TARGETS += qmltypes

INSTALLS += target qmldir
