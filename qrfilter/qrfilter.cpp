/**
 * @file qrfilter.cpp
 * @copyright 2020 Open Mobile Platform LLC.
 * @author Dmitry Butakov d.butakov@omprussia.ru
 */

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

void QrFilter::setResult(const QString &result)
{
    if (m_result == result)
        return;

    m_result = result;
    emit resultChanged(m_result);
}

void QrFilter::clearResult()
{
    setResult(QString());
}
