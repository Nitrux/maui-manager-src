#include "accessibilitymanager.h"
#include "accessibility_interface.h"
#include "mauimanutils.h"

#include <QDebug>
#include <QDBusMessage>

using namespace MauiMan;

AccessibilityManager::AccessibilityManager(QObject *parent) : QObject(parent)
{
    auto server = MauiManUtils::instance();
    if(server->serverRunning())
    {
        this->setConnections();
    }

    connect(server, &MauiManUtils::serverRunningChanged, this, [this](bool state)
    {
        if(state)
        {
            this->setConnections();
        }
    });

    loadSettings();
}

bool AccessibilityManager::singleClick() const
{
    if (m_interface && m_interface->isValid())
    {
        const QVariant value = m_interface->property("singleClick");
        if (value.isValid())
        {
            return value.toBool();
        }
    }

    return m_singleClick;
}

void AccessibilityManager::setSingleClick(bool singleClick)
{
    if (!m_interface || !m_interface->isValid())
    {
        qWarning() << "AccessibilityManager::setSingleClick ignored because Accessibility facade is read-only without an active MauiMan D-Bus interface.";
        return;
    }

    if (this->singleClick() == singleClick)
        return;

    sync(QStringLiteral("setSingleClick"), singleClick);
    loadSettings();
}

void AccessibilityManager::onSingleClickChanged(bool singleClick)
{
    Q_UNUSED(singleClick);
    const bool previousSingleClick = m_singleClick;

    loadSettings();
    if (m_singleClick == previousSingleClick)
        return;

    Q_EMIT singleClickChanged(m_singleClick);
}

void AccessibilityManager::onScrollBarPolicyChanged(uint scrollBarPolicy)
{
    Q_UNUSED(scrollBarPolicy);
    const uint previousScrollBarPolicy = m_scrollBarPolicy;

    loadSettings();
    if (m_scrollBarPolicy == previousScrollBarPolicy)
        return;

    Q_EMIT scrollBarPolicyChanged(m_scrollBarPolicy);
}

void AccessibilityManager::onPlaySoundsChanged(bool playSounds)
{
    Q_UNUSED(playSounds);
    const bool previousPlaySounds = m_playSounds;

    loadSettings();
    if (m_playSounds == previousPlaySounds)
        return;

    Q_EMIT playSoundsChanged(m_playSounds);
}

void AccessibilityManager::sync(const QString &key, const QVariant &value)
{
    if (m_interface && m_interface->isValid())
    {
        const QDBusMessage reply = m_interface->call(key, value);
        if (reply.type() == QDBusMessage::ErrorMessage)
        {
            qWarning() << "AccessibilityManager::sync failed for call" << key << ":" << reply.errorMessage();
        }
        return;
    }

    qWarning() << "AccessibilityManager::sync skipped (no valid org.mauiman.Accessibility interface) for call:" << key;
}

void AccessibilityManager::setConnections()
{
    if(m_interface)
    {
        m_interface->disconnect();
        m_interface->deleteLater();
        m_interface = nullptr;
    }

    m_interface = new OrgMauimanAccessibilityInterface(QStringLiteral("org.mauiman.Manager"),
                                                       QStringLiteral("/Accessibility"),
                                                       QDBusConnection::sessionBus(),
                                                       this);

    if (m_interface->isValid())
    {
        connect(m_interface, &OrgMauimanAccessibilityInterface::singleClickChanged, this, &AccessibilityManager::onSingleClickChanged);
        connect(m_interface, &OrgMauimanAccessibilityInterface::playSoundsChanged, this, &AccessibilityManager::onPlaySoundsChanged);
        connect(m_interface, &OrgMauimanAccessibilityInterface::scrollBarPolicyChanged, this, &AccessibilityManager::onScrollBarPolicyChanged);
        loadSettings();
    }
}

void AccessibilityManager::loadSettings()
{
    if(m_interface && m_interface->isValid())
    {
        const QVariant singleClick = m_interface->property("singleClick");
        if (singleClick.isValid())
        {
            m_singleClick = singleClick.toBool();
        }

        const QVariant scrollBarPolicy = m_interface->property("scrollBarPolicy");
        if (scrollBarPolicy.isValid())
        {
            m_scrollBarPolicy = scrollBarPolicy.toUInt();
        }

        const QVariant playSounds = m_interface->property("playSounds");
        if (playSounds.isValid())
        {
            m_playSounds = playSounds.toBool();
        }
    }
}

uint AccessibilityManager::scrollBarPolicy() const
{
    if (m_interface && m_interface->isValid())
    {
        const QVariant value = m_interface->property("scrollBarPolicy");
        if (value.isValid())
        {
            return value.toUInt();
        }
    }

    return m_scrollBarPolicy;
}

void AccessibilityManager::setScrollBarPolicy(uint newScrollBarPolicy)
{
    if (!m_interface || !m_interface->isValid())
    {
        qWarning() << "AccessibilityManager::setScrollBarPolicy ignored because Accessibility facade is read-only without an active MauiMan D-Bus interface.";
        return;
    }

    if (this->scrollBarPolicy() == newScrollBarPolicy)
        return;

    sync(QStringLiteral("setScrollBarPolicy"), newScrollBarPolicy);
    loadSettings();
}

bool AccessibilityManager::playSounds() const
{
    if (m_interface && m_interface->isValid())
    {
        const QVariant value = m_interface->property("playSounds");
        if (value.isValid())
        {
            return value.toBool();
        }
    }

    return m_playSounds;
}

void AccessibilityManager::setPlaySounds(bool newPlaySounds)
{
    if (!m_interface || !m_interface->isValid())
    {
        qWarning() << "AccessibilityManager::setPlaySounds ignored because Accessibility facade is read-only without an active MauiMan D-Bus interface.";
        return;
    }

    if (this->playSounds() == newPlaySounds)
        return;

    sync(QStringLiteral("setPlaySounds"), newPlaySounds);
    loadSettings();
}
