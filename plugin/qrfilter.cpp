#include "qrfilter.h"
#include <QByteArray>
#include <QDebug>
#include <QFuture>
#include <QtConcurrent>
#include <ZXing/ReadBarcode.h>
#include <ZXing/DecodeHints.h>
#include <ZXing/Result.h>

class VideoFilterRunnable : public QVideoFilterRunnable
{
public:
    explicit VideoFilterRunnable(QrFilter *filter);
    ~VideoFilterRunnable() override;

    QVideoFrame run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat,
                    RunFlags flags) override;

private:
    static void analyze(VideoFilterRunnable *runnable);
    QrFilter *m_filter = nullptr;
    QFuture<void> m_future;
    QVideoSurfaceFormat m_surfaceFormat;
    QByteArray m_buffer;
};

VideoFilterRunnable::VideoFilterRunnable(QrFilter *filter)
    : m_filter(filter)
{
}

VideoFilterRunnable::~VideoFilterRunnable()
{
    if (m_future.isRunning()) {
        m_future.waitForFinished();
    }
}

QVideoFrame VideoFilterRunnable::run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat,
                                     QVideoFilterRunnable::RunFlags flags)
{
    Q_UNUSED(flags)

    if (!input || !input->isValid()) {
        qWarning() << "Invalid input video frame!";
        return {};
    }

    if (m_future.isRunning() || !m_filter->m_enabled) {
        return *input;
    }

    if (input->map(QAbstractVideoBuffer::ReadOnly)) {
        int mappedSize = input->mappedBytes();
        auto pBits = reinterpret_cast<char*>(input->bits());

        m_buffer.replace(0, m_buffer.size(), pBits, mappedSize);
        input->unmap();

        m_surfaceFormat = surfaceFormat;
        m_future = QtConcurrent::run(analyze, this);
    }

    return *input;
}

void VideoFilterRunnable::analyze(VideoFilterRunnable *runnable)
{
    ZXing::DecodeHints hints;
    ZXing::Result result = ZXing::ReadBarcode(
    { reinterpret_cast<uchar*>(runnable->m_buffer.data()),
                               runnable->m_surfaceFormat.frameWidth(),
                               runnable->m_surfaceFormat.frameHeight(),
                               ZXing::ImageFormat::RGBX
    }, hints);
    runnable->m_filter->setResult(QString::fromStdWString(result.text()));
}

QrFilter::QrFilter(QObject *parent)
    : QAbstractVideoFilter(parent)
{
}

QVideoFilterRunnable *QrFilter::createFilterRunnable()
{
    return new VideoFilterRunnable(this);
}

QString QrFilter::result() const
{
    return m_result;
}

void QrFilter::clearResult()
{
    m_result.clear();
    emit resultChanged(m_result);
}

void QrFilter::setResult(const QString &result)
{
    if (!m_enabled || result.isEmpty() || m_result == result)
        return;

    m_result = result;
    emit resultChanged(m_result);
}

bool QrFilter::enabled() const
{
    return m_enabled;
}

void QrFilter::setEnabled(bool enabled)
{
    if (m_enabled == enabled)
        return;

    m_enabled = enabled;
    emit enabledChanged(m_enabled);
}


