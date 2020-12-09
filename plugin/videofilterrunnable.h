/**
 * @file videofilterrunnable.cpp
 * @copyright 2020 Open Mobile PLatform Ltd.
 * @author Dmitry Butakov d.butakov@omprussia.ru
 */

#ifndef VIDEOFILTERRUNNABLE_H
#define VIDEOFILTERRUNNABLE_H

#include "qrfilter.h"

#include <QtDBus/QDBusUnixFileDescriptor>
#include <QtDBus/QDBusPendingReply>
#include <QVideoFilterRunnable>

class VideoFilterRunnable : public QObject, public QVideoFilterRunnable
{
    Q_OBJECT
public:
    explicit VideoFilterRunnable(QrFilter *filter);
    ~VideoFilterRunnable() override = default;

    QVideoFrame run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat,
                    RunFlags flags) override;

signals:
    void stringFound(const QString &result);

private:
    void analyze();
    QrFilter *m_filter = nullptr;
    QVideoSurfaceFormat m_surfaceFormat;
    QDBusPendingReply<QString> m_reply;
    QDBusUnixFileDescriptor m_fd;
    size_t m_bufferSize;
    int m_frameCounter;
};

#endif // VIDEOFILTERRUNNABLE_H
