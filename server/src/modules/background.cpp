#include "background.h"
#include "backgroundadaptor.h"

#include <QDBusConnection>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(LOG_BACKGROUND_SERVER, "mauiman.server.background")

Background::Background(QObject *parent)
    : BackgroundBase(parent)
{
    qCInfo(LOG_BACKGROUND_SERVER, "INIT BACKGROUND MODULE");
    new BackgroundAdaptor(this);
    if (!QDBusConnection::sessionBus().registerObject(QStringLiteral("/Background"), this))
    {
        qCWarning(LOG_BACKGROUND_SERVER) << "FAILED TO REGISTER BACKGROUND DBUS OBJECT";
        return;
    }
    loadAllPrefs();
}
