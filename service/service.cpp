/**
 * @file service.cpp
 * @copyright 2020 Open Mobile Platform LLC.
 * @copyright 2026 Jolla Mobile Ltd
 * @author Dmitry Butakov d.butakov@omprussia.ru
 */

#include "service.h"
#include "zxing_adaptor.h"

#include <QDBusContext>
#include <QDBusConnectionInterface>
#include <QDBusReply>
#include <QDBusMessage>
#include <QDebug>
#include <QFile>
#include <QVideoSurfaceFormat>

#include <ZXing/BarcodeFormat.h>
#include <ZXing/ReadBarcode.h>
#include <ZXing/ReaderOptions.h>
#include <ZXing/Barcode.h>

#include <unistd.h>
#include <sys/mman.h>

const char *SERVICE_NAME = "org.amberapi.zxing";
const char *OBJECT_PATH = "/org/amberapi/zxing";

Service::Service(QObject *parent)
    : QObject(parent),
      QDBusContext()
{
    new ZxingAdaptor(this);

    QDBusConnection connection = QDBusConnection::sessionBus();
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

    m_autoclose.start();
}

Service::~Service()
{
}

static ZXing::ImageFormat convertFormat(int pixelFormat)
{
    switch (pixelFormat) {
    case QVideoFrame::Format_ARGB32:
        return ZXing::ImageFormat::ARGB;
    case QVideoFrame::Format_ARGB32_Premultiplied:
        return ZXing::ImageFormat::ARGB;
    case QVideoFrame::Format_RGB32:
        return ZXing::ImageFormat::RGB;
    case QVideoFrame::Format_BGRA32:
        return ZXing::ImageFormat::BGRA;
    case QVideoFrame::Format_BGRA32_Premultiplied:
        return ZXing::ImageFormat::BGRA;
    case QVideoFrame::Format_BGR32:
        return ZXing::ImageFormat::BGR;
    default:
        return ZXing::ImageFormat::None;
    }
}

QString Service::decodeFromDescriptor(QDBusUnixFileDescriptor fd,
                                  uint size, int width, int height, int pixelFormat)
{
    m_autoclose.stop();
    QString response;
    QFile mf;
    if (mf.open(fd.fileDescriptor(), QIODevice::ReadOnly)) {
        uchar *buf = mf.map(0, size);

        if (buf != nullptr) {
            ZXing::ReaderOptions options;
            options.setFormats(ZXing::BarcodeFormat::QRCode);
            ZXing::ImageFormat format = convertFormat(pixelFormat);
            options.setTryInvert(true);

            if (format != ZXing::ImageFormat::None) {
                ZXing::ImageView image(buf, width, height, format);
                ZXing::Barcode barcode = ZXing::ReadBarcode(image, options);
                response = QString::fromStdString(barcode.text());
            } else {
                qWarning() << "Input frame format is not supported by ZXing: "
                           << pixelFormat;
            }
        } else {
            qWarning() << "map():" << mf.error();
        }
    } else {
        qWarning() << "open():" << mf.error();
    }

    m_autoclose.start();
    return response;
}

void Service::quit()
{
    QCoreApplication::quit();
}
