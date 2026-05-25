#include "inputdevices.h"
#include "inputdevicesadaptor.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(LOG_INPUTDEVICES_SERVER, "mauiman.server.inputdevices")

InputDevices::InputDevices(QObject *parent) : QObject(parent)
{
    qCInfo(LOG_INPUTDEVICES_SERVER, "INIT INPUT_DEVICES MODULE");

    new InputDevicesAdaptor(this);
    if(!QDBusConnection::sessionBus().registerObject(QStringLiteral("/InputDevices"), this))
    {
        qCWarning(LOG_INPUTDEVICES_SERVER) << "FAILED TO REGISTER INPUT_DEVICES DBUS OBJECT";
        return;
    }

    // Read-only bootstrap from common wlroots/XKB environment exports.
    m_keyboardLayout = qEnvironmentVariable("XKB_DEFAULT_LAYOUT", m_keyboardLayout);
    m_keyboardModel = qEnvironmentVariable("XKB_DEFAULT_MODEL", m_keyboardModel);
    m_keyboardVariant = qEnvironmentVariable("XKB_DEFAULT_VARIANT", m_keyboardVariant);
    m_keyboardOptions = qEnvironmentVariable("XKB_DEFAULT_OPTIONS", m_keyboardOptions);
    m_keyboardRules = qEnvironmentVariable("XKB_DEFAULT_RULES", m_keyboardRules);
    m_kdeLayoutShortNames = m_keyboardLayout.split(QLatin1Char(','), Qt::SkipEmptyParts);
    if (!m_kdeLayoutShortNames.isEmpty())
    {
        m_keyboardLayout = m_kdeLayoutShortNames.constFirst().trimmed();
    }

    // TODO: Add non-KDE live backends for compositor-independent environments.
    // Candidates:
    // 1) KWin/plasma keyboard D-Bus integration (implemented below).
    // 2) wlroots/sway IPC keymap observers.
    // 3) Session-specific seat/keymap provider used by Maui Shell.
    //
    // Expected behavior:
    // - Emit keyboard*Changed signals on runtime keymap changes without restart.
    // - Keep property values aligned with compositor state, not local cache writes.
    setupKdeLayoutBridge();
}

QString InputDevices::keyboardLayout() const
{
    return m_keyboardLayout;
}

QString InputDevices::keyboardModel() const
{
    return m_keyboardModel;
}

QString InputDevices::keyboardVariant() const
{
    return m_keyboardVariant;
}

QString InputDevices::keyboardOptions() const
{
    return m_keyboardOptions;
}

QString InputDevices::keyboardRules() const
{
    return m_keyboardRules;
}

void InputDevices::setupKdeLayoutBridge()
{
    m_kdeLayoutsInterface = new QDBusInterface(QStringLiteral("org.kde.keyboard"),
                                               QStringLiteral("/Layouts"),
                                               QStringLiteral("org.kde.KeyboardLayouts"),
                                               QDBusConnection::sessionBus(),
                                               this);

    if (!m_kdeLayoutsInterface->isValid())
    {
        return;
    }

    refreshCurrentKdeLayout();

    QDBusConnection::sessionBus().connect(QStringLiteral("org.kde.keyboard"),
                                          QStringLiteral("/Layouts"),
                                          QStringLiteral("org.kde.KeyboardLayouts"),
                                          QStringLiteral("layoutChanged"),
                                          this,
                                          SLOT(onKdeLayoutChanged(uint)));

    QDBusConnection::sessionBus().connect(QStringLiteral("org.kde.keyboard"),
                                          QStringLiteral("/Layouts"),
                                          QStringLiteral("org.kde.KeyboardLayouts"),
                                          QStringLiteral("layoutListChanged"),
                                          this,
                                          SLOT(onKdeLayoutListChanged()));
}

void InputDevices::onKdeLayoutChanged(uint index)
{
    refreshCurrentKdeLayout(static_cast<int>(index));
}

void InputDevices::onKdeLayoutListChanged()
{
    refreshCurrentKdeLayout();
}

QString InputDevices::layoutFromIndex(int index) const
{
    if (index >= 0 && index < m_kdeLayoutShortNames.size())
    {
        return m_kdeLayoutShortNames.at(index);
    }

    return QString();
}

void InputDevices::refreshCurrentKdeLayout(int preferredIndex)
{
    if (!m_kdeLayoutsInterface || !m_kdeLayoutsInterface->isValid())
    {
        return;
    }

    const QDBusReply<QString> currentLayoutName = m_kdeLayoutsInterface->call(QStringLiteral("getCurrentLayout"));
    if (currentLayoutName.isValid())
    {
        const QString layoutName = currentLayoutName.value().trimmed();
        if (!layoutName.isEmpty() && layoutName != m_keyboardLayout)
        {
            m_keyboardLayout = layoutName;
            Q_EMIT keyboardLayoutChanged(m_keyboardLayout);
        }

        if (!layoutName.isEmpty())
        {
            return;
        }
    }

    int index = preferredIndex;
    if (index < 0)
    {
        const QDBusReply<uint> currentLayout = m_kdeLayoutsInterface->call(QStringLiteral("getLayout"));
        if (!currentLayout.isValid())
        {
            if (!m_missingKdeLayoutsWarned)
            {
                m_missingKdeLayoutsWarned = true;
                qCWarning(LOG_INPUTDEVICES_SERVER) << "InputDevices facade could not query org.kde.KeyboardLayouts.getLayout(); keeping current keyboardLayout value.";
            }
            return;
        }

        index = static_cast<int>(currentLayout.value());
    }

    const QString mappedLayout = layoutFromIndex(index);
    if (mappedLayout.isEmpty() || mappedLayout == m_keyboardLayout)
    {
        return;
    }

    m_keyboardLayout = mappedLayout;
    Q_EMIT keyboardLayoutChanged(m_keyboardLayout);
}
