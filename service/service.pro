TARGET = zxing-daemon

QT = core dbus
CONFIG += link_pkgconfig
PKGCONFIG += zxing

HEADERS = \
    service.h

SOURCES = \
    service.cpp \
    main.cpp

target.path = /usr/bin/

DBUS_SERVICE_NAME=org.sailfishos.zxing

zxing_dbus_adaptor.files = ./dbus/$${DBUS_SERVICE_NAME}.xml
zxing_dbus_adaptor.source_flags = -c ZxingAdaptor
DBUS_ADAPTORS += zxing_dbus_adaptor

systemd.files = ./systemd/dbus-$${DBUS_SERVICE_NAME}.service
systemd.path = /usr/lib/systemd/system/

service.files = ./dbus/$${DBUS_SERVICE_NAME}.service
service.path = /usr/share/dbus-1/system-services/

conf.files = ./dbus/$${DBUS_SERVICE_NAME}.conf
conf.path = /etc/dbus-1/system.d/

INSTALLS += systemd service conf target
