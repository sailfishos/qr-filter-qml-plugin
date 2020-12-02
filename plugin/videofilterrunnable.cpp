#include "videofilterrunnable.h"

#include <QDebug>
#include <QtDBus/QDBusConnection>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define FRAME_ANALYSIS_RATE 10

VideoFilterRunnable::VideoFilterRunnable(QrFilter *filter)
    : m_filter(filter), m_frameCounter(0)
{
    connect(this, &VideoFilterRunnable::stringFound,
            m_filter, &QrFilter::setResult, Qt::QueuedConnection);
}

static QString getRandomString()
{
   const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                    "abcdefghijklmnopqrstuvwxyz0123456789");
   const int randomStringLength = 12;

   QString randomString;
   for(int i = 0; i < randomStringLength; ++i)
   {
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

    if (m_reply.isValid()) {
        if (m_reply.isFinished()) {
            if (m_reply.isError()) {
                qWarning() << "QR Filter error" << m_reply.error().message();
            } else {
                QString result = m_reply.value();
                if (!result.isEmpty()) {
                    emit stringFound(result);
                }
            }
            m_reply = QDBusPendingReply<QString>();
        } else {
            return *input;
        }
    }

    if (!input || !input->isValid()) {
        qWarning() << "Invalid input video frame!";
        return {};
    }

    if (m_frameCounter < FRAME_ANALYSIS_RATE) {
        ++m_frameCounter;
        return *input;
    }

    m_frameCounter = 0;
    if (input->map(QAbstractVideoBuffer::ReadOnly)) {
        auto pBits = reinterpret_cast<char*>(input->bits());
        m_bufferSize = static_cast<uint>(input->mappedBytes());

        // Genereate tmp frame buffer and unlink immediatly to avoid frame collision
        const char *tmpFrameBufferName =  getRandomString().toStdString().c_str();
        int fd = shm_open(tmpFrameBufferName, O_RDWR | O_CREAT, 0777);

        if (fd != -1) {
            shm_unlink(tmpFrameBufferName);
            if (write(fd, pBits, m_bufferSize) != -1) {
                m_surfaceFormat = surfaceFormat;
                m_fd.setFileDescriptor(fd);
                analyze();
            } else {
                 qDebug() << "write():" << strerror(errno);
            }
            close(fd);
        } else {
            qDebug() << "shm_open():" << strerror(errno);
        }

        input->unmap();
    }

    return *input;
}

void VideoFilterRunnable::analyze()
{
    QDBusMessage message = QDBusMessage::createMethodCall("org.sailfishos.zxing",
                                                          "/org/sailfishos/zxing",
                                                          "org.sailfishos.zxing",
                                                          "decodeFromMemory");
    QList<QVariant> args;
    args.append(QVariant::fromValue(m_fd));
    args.append(m_bufferSize);
    args.append(m_surfaceFormat.frameWidth());
    args.append(m_surfaceFormat.frameHeight());
    message.setArguments(args);
    m_reply =  QDBusConnection::systemBus().asyncCall(message);
}
