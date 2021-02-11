/**
 * @file main.cpp
 * @copyright 2020 Open Mobile Platform LLC.
 * @author Dmitry Butakov d.butakov@omprussia.ru
 */

#include "service.h"

#include <QCoreApplication>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    Service srv;

    return app.exec();
}
