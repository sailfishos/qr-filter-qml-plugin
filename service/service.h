/**
 * @file service.h
 * @copyright 2020 Open Mobile PLatform Ltd.
 * @author Dmitry Butakov d.butakov@omprussia.ru
 * @date 2020
 */

#ifndef SERVICE_H
#define SERVICE_H

#include <QDBusContext>
#include <QDBusUnixFileDescriptor>
#include <QTimer>

class Service: public QObject, public QDBusContext
{
    Q_OBJECT

public:
    explicit Service(QObject *parent = nullptr);
    virtual ~Service();

public slots:
    QString decodeFromMemory(QDBusUnixFileDescriptor fd, uint size, int width, int height);
    void quit();

private:
    QTimer m_autoclose;

    static const char *SERVICE_NAME;
    static const char *OBJECT_PATH;
};

#endif // SERVICE_H