/**
 * @file service.cpp
 * @copyright 2020 Open Mobile Platform LLC.
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
#include <ZXing/DecodeHints.h>
#include <ZXing/Result.h>

#include <QImage>
#include <BitMatrix.h>
#include <BitMatrixIO.h>
#include <ZXing/CharacterSet.h>
#include <ZXing/MultiFormatWriter.h>

#include <unistd.h>
#include <sys/mman.h>

const char *SERVICE_NAME = "org.amberapi.zxing";
const char *OBJECT_PATH = "/org/amberapi/zxing";

ZXing::BitMatrix textToMatrix(QString text, int width, int height, int margin, ZXing::BarcodeFormat format=ZXing::BarcodeFormat::QRCode)
{
    ZXing::MultiFormatWriter writer(format);
    writer.setEncoding(ZXing::CharacterSet::UTF8);
    if (margin >= 0) {
        writer.setMargin(margin);
    }
    return writer.encode(text.toStdString(), width, height);
}

QImage textToImage(QString text, int width, int height, int margin)
{
    ZXing::BitMatrix matrix = textToMatrix(text, width, height, margin);
    auto bitmap = ZXing::ToMatrix<uint8_t>(matrix);
    QImage image = QImage(bitmap.data(), bitmap.width(), bitmap.height(), bitmap.width(), QImage::Format::Format_Grayscale8).copy();
    return image;
}

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
        return ZXing::ImageFormat::XRGB;
    case QVideoFrame::Format_ARGB32_Premultiplied:
        return ZXing::ImageFormat::XRGB;
    case QVideoFrame::Format_RGB32:
        return ZXing::ImageFormat::RGB;
    case QVideoFrame::Format_BGRA32:
        return ZXing::ImageFormat::BGRX;
    case QVideoFrame::Format_BGRA32_Premultiplied:
        return ZXing::ImageFormat::BGRX;
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
            ZXing::DecodeHints hints;
            hints.setFormats(ZXing::BarcodeFormat::QRCode);
            ZXing::ImageFormat format = convertFormat(pixelFormat);
            hints.setTryInvert(true);

            if (format != ZXing::ImageFormat::None) {
                ZXing::Result result = ZXing::ReadBarcode(
                { buf, width, height, format}, hints);
                response = QString::fromStdString(result.text());
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

bool Service::encodeToDescriptor(const QDBusUnixFileDescriptor &fd,
                             const QString &text, int width, int height, int margin)
{
    m_autoclose.stop();
    bool response = false;
    QImage image = textToImage(text, width, height, margin);
    QFile mf;
    if (mf.open(fd.fileDescriptor(), QIODevice::WriteOnly)) {
        response = image.save(&mf, "PNG");
        if (!response) {
            qWarning() << "QImage::save failed.";
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

