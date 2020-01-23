#ifndef QRFILTER_H
#define QRFILTER_H

#include <QAbstractVideoFilter>

class QrFilter : public QAbstractVideoFilter
{
    Q_OBJECT
    Q_PROPERTY(QString result READ result NOTIFY resultChanged)

public:
    explicit QrFilter(QObject *parent = nullptr);
    virtual ~QrFilter() = default;

    QVideoFilterRunnable *createFilterRunnable() override;
    QString result() const;
    Q_INVOKABLE void clearResult();
    void setResult(const QString &result);

signals:
    void decodeFinished(QString result);
    void resultChanged(QString result);

private:
    QString m_result;
    friend class VideoFilterRunnable;
};

#endif // QRFILTER_H
