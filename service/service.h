/**
 * @file service.h
 * @copyright 2020 Open Mobile Platform LLC.
 * @author Dmitry Butakov d.butakov@omprussia.ru
 */

#ifndef SERVICE_H
#define SERVICE_H

#include <QDBusContext>
#include <QDBusUnixFileDescriptor>
#include <QTimer>
#include <QString>

class Service: public QObject, public QDBusContext
{
    Q_OBJECT

public:
    explicit Service(QObject *parent = nullptr);
    virtual ~Service();

public slots:
    QString decodeFromDescriptor(QDBusUnixFileDescriptor fd,
            uint size, int width, int height, int pixelFormat);
    bool    encodeToDescriptor(const QDBusUnixFileDescriptor &fd,
            const QString &text, int width, int height, int margin);
    void quit();

private:
    QTimer m_autoclose;
};

#endif // SERVICE_H
