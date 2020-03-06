#include "qrfilter.h"
#include <QByteArray>
#include <QDebug>
#include <QFuture>
#include <QtConcurrent>
#include <QZXing.h>

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
    QImage m_img;
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

        m_img = QImage(reinterpret_cast<uchar*>(m_buffer.data()), surfaceFormat.frameWidth(),
                       surfaceFormat.frameHeight(), QImage::Format_ARGB32);

        m_future = QtConcurrent::run(analyze, this);
    }

    return *input;
}

void VideoFilterRunnable::analyze(VideoFilterRunnable *runnable)
{
    QZXing decoder;
    decoder.setDecoder(QZXing::DecoderFormat_QR_CODE);
    QString result = decoder.decodeImage(runnable->m_img);
    runnable->m_filter->setResult(result);
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


