#include "theme.h"
#include "themeadaptor.h"

#include <QDBusConnection>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(LOG_THEME_SERVER, "mauiman.server.theme")

Theme::Theme(QObject *parent)
    : ThemeBase(parent)
{
    qCInfo(LOG_THEME_SERVER, "INIT THEME SERVER");
    new ThemeAdaptor(this);
    if (!QDBusConnection::sessionBus().registerObject(QStringLiteral("/Theme"), this))
    {
        qCWarning(LOG_THEME_SERVER) << "FAILED TO REGISTER THEME DBUS OBJECT";
        return;
    }
    loadAllPrefs();
}
