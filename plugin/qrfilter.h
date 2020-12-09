/**
 * @file qrfilter.h
 * @copyright 2020 Open Mobile PLatform Ltd.
 * @author Dmitry Butakov d.butakov@omprussia.ru
 */

#ifndef QRFILTER_H
#define QRFILTER_H

#include <QAbstractVideoFilter>

class QrFilter : public QAbstractVideoFilter
{
    Q_OBJECT
    Q_PROPERTY(QString result READ result NOTIFY resultChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

public:
    explicit QrFilter(QObject *parent = nullptr);
    virtual ~QrFilter() override = default;

    QVideoFilterRunnable *createFilterRunnable() override;
    QString result() const;
    Q_INVOKABLE void clearResult();
    void setResult(const QString &result);
    bool enabled() const;
    void setEnabled(bool enabled);

signals:
    void decodeFinished(const QString &result);
    void resultChanged(const QString &result);
    void enabledChanged(bool enabled);

private:
    QString m_result;
    bool m_enabled = true;
    friend class VideoFilterRunnable;
};

#endif // QRFILTER_H
