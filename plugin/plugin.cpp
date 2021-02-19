/**
 * @file plugin.cpp
 * @copyright 2020 Open Mobile Platform LLC.
 * @author Dmitry Butakov d.butakov@omprussia.ru
 */

#include <QQmlExtensionPlugin>
#include <QtQml>

#include "qrfilter.h"

class Q_DECL_EXPORT QrFilterPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.sailfishos.qrfilter")

public:
    QrFilterPlugin() = default;
    virtual ~QrFilterPlugin() = default;

    void registerTypes(const char *uri) override
    {
        Q_ASSERT(QLatin1String(uri) == QStringLiteral("org.sailfishos.qrfilter"));
        qmlRegisterType<QrFilter>(uri, 1, 0, "QrFilter");
    }
};

#include "plugin.moc"
