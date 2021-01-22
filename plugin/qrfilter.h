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

public:
    explicit QrFilter(QObject *parent = nullptr);
    virtual ~QrFilter() override = default;

    QVideoFilterRunnable *createFilterRunnable() override;
    QString result() const;
    void setResult(const QString &result);

    Q_INVOKABLE void clearResult();

signals:
    void decodeFinished(const QString &result);
    void resultChanged(const QString &result);

private:
    QString m_result;
    friend class VideoFilterRunnable;
};

#endif // QRFILTER_H
