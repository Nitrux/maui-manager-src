#pragma once

#include <QObject>
#include "mauiman_export.h"

/**
 * @brief The MauiManUtils class
 */
class MAUIMAN_EXPORT MauiManUtils : public QObject
{
    Q_OBJECT
    /**
     * Whether the server application is running, listening and broadcasting events.
     */
    Q_PROPERTY(bool serverRunning READ serverRunning NOTIFY serverRunningChanged)

public:
    explicit MauiManUtils(QObject *parent = nullptr);
    static MauiManUtils *instance();

    bool serverRunning() const;

    /**
     * @brief Invoke the `MauiManServer` application to be launched.
     */
    static void startServer();

    /**
     * @brief Invoke the `MauiSettings` application to be launched at a given module name
     * @param module the name of the module to be launched
     */
    static void invokeManager(const QString &module);

Q_SIGNALS:
    void serverRunningChanged(bool state);

private:
    bool m_serverRunning = false;
};
