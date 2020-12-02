/**
 * @file main.cpp
 * @copyright 2020 Open Mobile PLatform Ltd.
 * @author Dmitry Butakov d.butakov@omprussia.ru
 * @date 2020
 */

#include "service.h"

#include <QCoreApplication>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Open Mobile Platform");
    QCoreApplication::setOrganizationDomain("https://omprussia.ru");
    QCoreApplication::setApplicationName("zxing-daemon");

    Service srv;

    app.exec();
    return 0;
}
