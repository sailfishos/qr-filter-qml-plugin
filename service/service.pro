TARGET = zxing-daemon

QT = core dbus multimedia
CONFIG += link_pkgconfig
PKGCONFIG += zxing

HEADERS = \
    service.h

SOURCES = \
    service.cpp \
    main.cpp

target.path = /usr/libexec

DBUS_SERVICE_NAME=org.amberapi.zxing

zxing_dbus_adaptor.files = ./dbus/$${DBUS_SERVICE_NAME}.xml
zxing_dbus_adaptor.source_flags = -c ZxingAdaptor
DBUS_ADAPTORS += zxing_dbus_adaptor

service.files = ./dbus/$${DBUS_SERVICE_NAME}.service
service.path = /usr/share/dbus-1/services/

INSTALLS += service target
