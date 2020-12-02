/**
 * @file service.cpp
 * @copyright 2020 Open Mobile PLatform Ltd.
 * @author Dmitry Butakov d.butakov@omprussia.ru
 * @date 2020
 */

#include "service.h"
#include "zxing_adaptor.h"

#include <QDBusContext>
#include <QDBusConnectionInterface>
#include <QDBusReply>
#include <QDBusMessage>
#include <QDebug>

#include <ZXing/ReadBarcode.h>
#include <ZXing/DecodeHints.h>
#include <ZXing/Result.h>

#include <unistd.h>
#include <sys/mman.h>

const char *Service::SERVICE_NAME = "org.sailfishos.zxing";
const char *Service::OBJECT_PATH = "/org/sailfishos/zxing";

Service::Service(QObject *parent)
    : QObject(parent),
      QDBusContext()
{
    QDBusConnection connection = QDBusConnection::systemBus();
    if (!connection.registerObject(OBJECT_PATH, this)) {
        qFatal("Cannot register object at %s", OBJECT_PATH);
    }

    if (!connection.registerService(SERVICE_NAME)) {
        qFatal("Cannot register D-Bus service at %s", SERVICE_NAME);
    }

    // prepare for controlled suicide on boredom
    const int m_autoclose_TIMEOUT_MS = 180 * 1000;

    m_autoclose.setSingleShot(true);
    m_autoclose.setInterval(m_autoclose_TIMEOUT_MS);

    connect(&m_autoclose, SIGNAL(timeout()),
            this, SLOT(quit()));

    new ZxingAdaptor(this);

    m_autoclose.start();
}

Service::~Service()
{
}

QString Service::decodeFromMemory(QDBusUnixFileDescriptor fd,
                                  uint size, int width, int height)
{
    m_autoclose.stop();
    QString response;
    uchar *buf = static_cast<uchar*>(mmap(nullptr, size,
                                               PROT_READ, MAP_SHARED, fd.fileDescriptor(), 0));

    if (buf != MAP_FAILED) {
        ZXing::DecodeHints hints;
        ZXing::Result result = ZXing::ReadBarcode(
        { buf, width, height, ZXing::ImageFormat::RGBX }, hints);
        response = QString::fromStdWString(result.text());
    } else {
        qWarning() << "decodFromMemory():" << strerror(errno);
    }

    m_autoclose.start();
    return response;
}

void Service::quit()
{
    QCoreApplication::quit();
}
