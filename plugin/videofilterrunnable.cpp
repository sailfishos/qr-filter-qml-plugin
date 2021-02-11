/**
 * @file videofilterrunnable.cpp
 * @copyright 2020 Open Mobile Platform LLC.
 * @author Dmitry Butakov d.butakov@omprussia.ru
 */

#include "videofilterrunnable.h"

#include <QDebug>
#include <QtDBus/QDBusConnection>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

VideoFilterRunnable::VideoFilterRunnable(QrFilter *filter) : m_filter(filter)
{
    connect(this, &VideoFilterRunnable::stringFound,
            m_filter, &QrFilter::setResult, Qt::QueuedConnection);
    m_timer.start();
    // analyze empty frame to establish D-Bus connection
    analyze(QDBusUnixFileDescriptor(0), 0,
            QVideoSurfaceFormat(QSize(0,0), QVideoFrame::PixelFormat::Format_BGRA32));
}

static QString getRandomString()
{
   const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                    "abcdefghijklmnopqrstuvwxyz0123456789");
   const int randomStringLength = 12;

   QString randomString;
   for (int i = 0; i < randomStringLength; ++i) {
       int index = qrand() % possibleCharacters.length();
       QChar nextChar = possibleCharacters.at(index);
       randomString.append(nextChar);
   }
   return randomString;
}

QVideoFrame VideoFilterRunnable::run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat,
                                     QVideoFilterRunnable::RunFlags flags)
{
    Q_UNUSED(flags)

    auto in = !input ? QVideoFrame() : *input;
    if (!m_reply.isFinished()) {
        return in;
    }

    if (m_reply.isError()) {
        qWarning() << "QR Filter error" << m_reply.error().message();
    } else {
        QString msg = m_reply.value();
        if (!msg.isEmpty()) {
            emit stringFound(msg);
        }
    }

    if (!in.isValid()) {
        return in;
    }

    if (!m_timer.hasExpired(500)) {
        return in;
    }
    m_timer.restart();
    m_reply = QDBusPendingReply<QString>();

    if (!in.map(QAbstractVideoBuffer::ReadOnly)) {
        return in;
    }

    auto pBits = reinterpret_cast<char*>(in.bits());
    auto bufferSize = static_cast<uint>(in.mappedBytes());

    // Generate tmp frame buffer and unlink immediately to avoid frame collision
    const char *tmpFrameBufferName = getRandomString().toStdString().c_str();
    int fd = shm_open(tmpFrameBufferName, O_RDWR | O_CREAT, 0777);

    if (fd != -1) {
        shm_unlink(tmpFrameBufferName);
        ssize_t wr = write(fd, pBits, bufferSize);
        if (wr > 0) {
            if (static_cast<size_t>(wr) == bufferSize) {
                QDBusUnixFileDescriptor dbus_fd(fd);
                analyze(dbus_fd, bufferSize, surfaceFormat);
            }  else {
                qDebug() << "Written " << wr << " bytes of" << bufferSize;
            }
        } else {
            qDebug() << "write():" << strerror(errno);
        }
        close(fd);
    } else {
        qDebug() << "shm_open():" << strerror(errno);
    }
    in.unmap();
    return in;
}

void VideoFilterRunnable::analyze(QDBusUnixFileDescriptor fd, uint bufferSize,
                                  QVideoSurfaceFormat surfaceFormat)
{
    QDBusMessage message = QDBusMessage::createMethodCall("org.sailfishos.zxing",
                                                          "/org/sailfishos/zxing",
                                                          "org.sailfishos.zxing",
                                                          "decodeFromDescriptor");
    QList<QVariant> args;
    args.append(QVariant::fromValue(fd));
    args.append(bufferSize);
    args.append(surfaceFormat.frameWidth());
    args.append(surfaceFormat.frameHeight());
    args.append(surfaceFormat.pixelFormat());
    message.setArguments(args);
    m_reply = QDBusConnection::sessionBus().asyncCall(message);
}
