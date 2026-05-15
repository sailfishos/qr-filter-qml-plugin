TARGET  = $$qtLibraryTarget(barcodeimage)

TEMPLATE = lib
CONFIG += plugin link_pkgconfig
PKGCONFIG += zxing

QT += core quick

include(translations.pri)

target.path = $$[QT_INSTALL_QML]/Amber/Barcode
qmldir.files += qmldir plugins.qmltypes
qmldir.path +=  $$target.path

SOURCES += \
        plugin.cpp \
        barcodeimage.cpp

HEADERS += \
        barcodeimage.h

qmltypes.commands = qmlplugindump -nonrelocatable Amber.Barcode 1.0 > $$PWD/plugins.qmltypes

QMAKE_EXTRA_TARGETS += qmltypes

INSTALLS += target qmldir translations_install engineering_english_install
