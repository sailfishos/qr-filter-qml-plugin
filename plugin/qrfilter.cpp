#include "qrfilter.h"
#include "videofilterrunnable.h"

QrFilter::QrFilter(QObject *parent)
    : QAbstractVideoFilter(parent)
{
}

QVideoFilterRunnable *QrFilter::createFilterRunnable()
{
    if (!isActive()) {
        return nullptr;
    }
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

    if (!enabled)
        clearResult();

    emit enabledChanged(m_enabled);
}
