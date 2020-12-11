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
    void analyze(QDBusUnixFileDescriptor fd, uint bufferSize,
                 QVideoSurfaceFormat surfaceFormat);
    QrFilter *m_filter = nullptr;
    QDBusPendingReply<QString> m_reply;
};

#endif // VIDEOFILTERRUNNABLE_H
