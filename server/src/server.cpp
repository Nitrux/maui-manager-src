#include "server.h"
#include <QDBusConnectionInterface>
#include <QLoggingCategory>

#include "modules/background.h"
#include "modules/theme.h"
#include "modules/screen.h"
#include "modules/formfactor.h"
#include "modules/accessibility.h"
#include "modules/inputdevices.h"

#include <signal.h>

Q_LOGGING_CATEGORY(LOG_SERVER, "mauiman.server")

// Note: qCInfo and QCoreApplication::exit() are not async-signal-safe;
// acceptable here for a session daemon but a self-pipe + QSocketNotifier
// would be the rigorous pattern.
void handleShutdownSignal(int signalNumber)
{
    qCInfo(LOG_SERVER) << "shutting down on signal" << signalNumber;
    if (QCoreApplication::instance())
        QCoreApplication::instance()->exit(0);
}

Server::Server(int &argc, char **argv) : QGuiApplication(argc, argv) {}

Server::~Server()
{
    qDeleteAll(m_modules);
}


bool Server::init()
{
    QDBusConnectionInterface *iface = QDBusConnection::sessionBus().interface();

    if(iface->isServiceRegistered(QStringLiteral("org.mauiman.Manager")))
    {
        qCWarning(LOG_SERVER) << "Service is already registered";
        return false;
    }

    auto registration = iface->registerService(QStringLiteral("org.mauiman.Manager"),
                                               QDBusConnectionInterface::ReplaceExistingService);

    if (!registration.isValid())
    {
        qCWarning(LOG_SERVER, "Failed to register D-Bus service \"%s\" on session bus: \"%s\"",
                  qPrintable(QStringLiteral("org.mauiman.Manager")),
                  qPrintable(registration.error().message()));

        return false;
    }    

    signal(SIGTERM, handleShutdownSignal);
    signal(SIGINT, handleShutdownSignal);

    // Persisted preference modules (read/write + settings-backed).
    m_modules << new Background();
    m_modules << new Theme();
    m_modules << new FormFactor();

    // Runtime facade modules (state mirrors platform/session signals).
    m_modules << new Screen();
    m_modules << new Accessibility();
    m_modules << new InputDevices();
    return true;
}
