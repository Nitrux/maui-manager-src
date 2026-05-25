#include "accessibility.h"

#include <QDBusConnection>
#include <QLoggingCategory>

#include "accessibilityadaptor.h"

Q_LOGGING_CATEGORY(LOG_ACCESSIBILITY_SERVER, "mauiman.server.accessibility")

Accessibility::Accessibility(QObject *parent) : QObject(parent)
{
    qCInfo(LOG_ACCESSIBILITY_SERVER, "INIT ACCESSIBILITY MODULE");

    new AccessibilityAdaptor(this);
    if (!QDBusConnection::sessionBus().registerObject(QStringLiteral("/Accessibility"), this))
    {
        qCWarning(LOG_ACCESSIBILITY_SERVER) << "FAILED TO REGISTER ACCESSIBILITY DBUS OBJECT";
        return;
    }
}

bool Accessibility::singleClick() const
{
    return m_singleClick;
}

uint Accessibility::scrollBarPolicy() const
{
    return m_scrollBarPolicy;
}

bool Accessibility::playSounds() const
{
    return m_playSounds;
}
