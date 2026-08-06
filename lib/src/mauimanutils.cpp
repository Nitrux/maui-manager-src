#include "mauimanutils.h"

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusConnectionInterface>
#include <QtDBus/QDBusServiceWatcher>

#include <QStringList>

#include <QProcess>

static const QString mauimanInterface(QStringLiteral("org.mauiman.Manager"));

MauiManUtils::MauiManUtils(QObject *parent)
: QObject{parent}
{
    const QDBusConnection bus = QDBusConnection::sessionBus();
    const auto registeredServices = bus.interface()->registeredServiceNames();

    if (registeredServices.isValid())
    {
        m_serverRunning = registeredServices.value().contains(mauimanInterface);
    }

    auto watcher = new QDBusServiceWatcher(mauimanInterface, QDBusConnection::sessionBus(),
                                           QDBusServiceWatcher::WatchForRegistration | QDBusServiceWatcher::WatchForUnregistration,
                                           this);

    connect(watcher, &QDBusServiceWatcher::serviceRegistered, this, [this] {
        m_serverRunning = true;
        Q_EMIT serverRunningChanged(m_serverRunning);
    });

    connect(watcher, &QDBusServiceWatcher::serviceUnregistered, this, [this] {
        m_serverRunning = false;
        Q_EMIT serverRunningChanged(m_serverRunning);
    });
}

MauiManUtils *MauiManUtils::instance()
{
    static MauiManUtils *s_instance = new MauiManUtils();
    return s_instance;
}

bool MauiManUtils::serverRunning() const
{
    return m_serverRunning;
}

void MauiManUtils::startServer()
{
    QProcess::startDetached(QStringLiteral("MauiManServer"), QStringList());
}

void MauiManUtils::invokeManager(const QString &module)
{
    QProcess::startDetached(QStringLiteral("MauiSettings"), QStringList {QStringLiteral("-m"), module});
}
