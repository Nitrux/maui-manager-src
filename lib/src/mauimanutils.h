#pragma once

#include <QObject>
#include "mauiman_export.h"

/**
 * @brief Tracks MauiMan service availability and launches its companion tools.
 *
 * instance() returns the process-wide watcher for org.mauiman.Manager on the
 * session bus. The launch helpers start detached processes and return immediately.
 */
class MAUIMAN_EXPORT MauiManUtils : public QObject
{
    Q_OBJECT
    /**
     * Whether the server application is running, listening and broadcasting events.
     */
    Q_PROPERTY(bool serverRunning READ serverRunning NOTIFY serverRunningChanged)

public:
    /** Creates a session-bus service watcher. Prefer instance() for shared use. */
    explicit MauiManUtils(QObject *parent = nullptr);
    /** Returns the process-wide MauiManUtils instance. */
    static MauiManUtils *instance();

    /** Returns whether org.mauiman.Manager is currently registered. */
    bool serverRunning() const;

    /** Starts MauiManServer as a detached process. */
    static void startServer();

    /** Opens MauiSettings directly at the named module. */
    static void invokeManager(const QString &module);

Q_SIGNALS:
    void serverRunningChanged(bool state);

private:
    bool m_serverRunning = false;
};
