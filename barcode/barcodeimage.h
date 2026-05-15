/**
 * @file barcodeimage.cpp
 * @copyright 2026 Jolla Mobile Ltd
 */

#ifndef BARCODEIMAGE_H
#define BARCODEIMAGE_H

#include <QObject>
#include <QPainter>
#include <QQuickPaintedItem>
#include <QString>

class BarcodeImage : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_PROPERTY(QString sourceText READ sourceText WRITE setSourceText NOTIFY sourceTextChanged)
    Q_PROPERTY(BarcodeFormat format READ format WRITE setFormat NOTIFY formatChanged)

public:
    BarcodeImage(QQuickItem *parent = nullptr);

    enum BarcodeFormat {
        Code128,
        QRCode,
    };
    Q_ENUM(BarcodeFormat)

    void paint(QPainter *painter) override;

    QString error();
    void setSourceText(const QString &text);
    QString sourceText();
    void setFormat(BarcodeFormat format);
    BarcodeFormat format();

signals:
    void errorChanged();
    void sourceTextChanged();
    void formatChanged();

private:
    QString m_error;
    QString m_text;
    BarcodeFormat m_format;

    void setError(const QString &newError);
};

#endif // BARCODEIMAGE_H
