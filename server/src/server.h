#pragma once

#include <QGuiApplication>
#include <QObject>
#include <QVector>

/**
 * @brief Runs the MauiMan session-bus service and owns its exported modules.
 *
 * init() claims org.mauiman.Manager and registers the Theme, FormFactor, and
 * InputDevices objects. Only one service instance can be active per session.
 */
class Server : public QGuiApplication
{
    Q_OBJECT
public:
    explicit Server(int &argc, char **argv);
    ~Server();
    /** Registers the service and its modules, returning false if registration fails. */
    bool init();

private:
    QVector<QObject*> m_modules;
};
