/**
 * @file barcodeimage.cpp
 * @copyright 2026 Jolla Mobile Ltd
 */

#include <QQmlExtensionPlugin>
#include <QQmlEngine>
#include <QtCore/QTranslator>
#include <QtCore/QLocale>

#include "barcodeimage.h"

// using custom translator so it gets properly removed from qApp when engine is deleted
class AppTranslator: public QTranslator
{
    Q_OBJECT

public:
    AppTranslator(QObject *parent)
        : QTranslator(parent)
    {
        qApp->installTranslator(this);
    }

    virtual ~AppTranslator()
    {
        qApp->removeTranslator(this);
    }
};

class Q_DECL_EXPORT BarcodeImagePlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Amber.Barcode")

public:
    void registerTypes(const char *uri) override
    {
        Q_ASSERT(QLatin1String(uri) == QStringLiteral("Amber.Barcode"));
        qmlRegisterType<BarcodeImage>(uri, 1, 0, "BarcodeImage");
    }

    void initializeEngine(QQmlEngine *engine, const char *uri)
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("Amber.Barcode"));

        AppTranslator *engineeringEnglish = new AppTranslator(engine);
        AppTranslator *translator = new AppTranslator(engine);
        engineeringEnglish->load("qr-filter-qml-plugin-barcode_eng_en", "/usr/share/translations");
        translator->load(QLocale(), "qr-filter-qml-plugin-barcode", "-", "/usr/share/translations");
    }
};

#include "plugin.moc"
