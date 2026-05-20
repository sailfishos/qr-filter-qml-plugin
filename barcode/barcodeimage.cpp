/**
 * @file barcodeimage.cpp
 * @copyright 2026 Jolla Mobile Ltd
 */

#include "barcodeimage.h"

#include <ZXing/BarcodeFormat.h>
#include <ZXing/CreateBarcode.h>
#include <ZXing/WriteBarcode.h>
#include <ZXing/ImageView.h>

#include <QImage>

static QString formatUnknownError(ZXing::BarcodeFormat fmt)
{
    switch (fmt) {
    case ZXing::BarcodeFormat::QRCode:
        //: Unknown error when creating a QR code
        //% "Unable to create QR code"
        return qtTrId("barcode-image-error-qrcode");
    case ZXing::BarcodeFormat::Code128:
    default:
        //: Unknown error when creating a barcode
        //% "Unable to create barcode"
        return qtTrId("barcode-image-error-barcode");
    }
}

BarcodeImage::BarcodeImage(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , m_format(Code128)
{
}

void BarcodeImage::paint(QPainter *painter)
{
    if (m_text.isEmpty()) {
        //: No data when getting barcode image
        //% "No data"
        setError(qtTrId("barcode-image-error-no-data"));
        return;
    }

    ZXing::BarcodeFormat format;
    switch (m_format) {
    case Code128:
        format = ZXing::BarcodeFormat::Code128;
        break;
    case QRCode:
    default:
        format = ZXing::BarcodeFormat::QRCode;
        break;
    }

    using namespace ZXing;

    CreatorOptions barcodeOptions = CreatorOptions(format);
    WriterOptions writerOptions = WriterOptions();

    try {
        Barcode barcode = CreateBarcodeFromText(m_text.toStdString(), barcodeOptions);
        ZXing::Image zxImage = ZXing::WriteBarcodeToImage(barcode, writerOptions);

        QImage image = QImage(zxImage.data(), zxImage.width(), zxImage.height(),
                              zxImage.width(), QImage::Format::Format_Grayscale8).copy();
        QRectF rect = contentsBoundingRect().adjusted(1, 1, -1, -1);
        painter->drawImage(rect, image);
        setError(QString());
    } catch (std::invalid_argument & e) {
        if (format == ZXing::BarcodeFormat::QRCode
            && strstr(e.what(), "Input too long")) {
            //: Too much data to create QR code or barcode
            //% "Too much data"
            setError(qtTrId("barcode-image-error-too-much-data"));
        } else {
            setError(formatUnknownError(format));
        }
    } catch (...) {
        setError(formatUnknownError(format));
    }
}

void BarcodeImage::setSourceText(const QString &text)
{
    if (m_text != text) {
        m_text = text;
        emit sourceTextChanged();
        update();
    }
}

QString BarcodeImage::sourceText()
{
    return m_text;
}

void BarcodeImage::setFormat(BarcodeFormat format)
{
    if (m_format != format) {
        m_format = format;
        emit formatChanged();
        update();
    }
}

BarcodeImage::BarcodeFormat BarcodeImage::format()
{
    return m_format;
}

void BarcodeImage::setError(const QString &newError)
{
    if (m_error != newError) {
        m_error = newError;
        emit errorChanged();
    }
}

QString BarcodeImage::error()
{
    return m_error;
}
