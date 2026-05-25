#include "inputdevicesmanager.h"
#include "inputdevices_interface.h"
#include "mauimanutils.h"

#include <QDebug>
#include <QDBusMessage>

using namespace MauiMan;
InputDevicesManager::InputDevicesManager(QObject *parent) : QObject(parent)
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

QString MauiMan::InputDevicesManager::keyboardLayout() const
{
    if (m_interface && m_interface->isValid())
    {
        const QVariant value = m_interface->property("keyboardLayout");
        if (value.isValid())
        {
            return value.toString();
        }
    }

    return m_keyboardLayout;
}

QString MauiMan::InputDevicesManager::keyboardModel() const
{
    if (m_interface && m_interface->isValid())
    {
        const QVariant value = m_interface->property("keyboardModel");
        if (value.isValid())
        {
            return value.toString();
        }
    }

    return m_keyboardModel;
}

QString MauiMan::InputDevicesManager::keyboardVariant() const
{
    if (m_interface && m_interface->isValid())
    {
        const QVariant value = m_interface->property("keyboardVariant");
        if (value.isValid())
        {
            return value.toString();
        }
    }

    return m_keyboardVariant;
}

QString MauiMan::InputDevicesManager::keyboardOptions() const
{
    if (m_interface && m_interface->isValid())
    {
        const QVariant value = m_interface->property("keyboardOptions");
        if (value.isValid())
        {
            return value.toString();
        }
    }

    return m_keyboardOptions;
}

QString MauiMan::InputDevicesManager::keyboardRules() const
{
    if (m_interface && m_interface->isValid())
    {
        const QVariant value = m_interface->property("keyboardRules");
        if (value.isValid())
        {
            return value.toString();
        }
    }

    return m_keyboardRules;
}

void MauiMan::InputDevicesManager::setKeyboardLayout(QString keyboardLayout)
{
    if (!m_interface || !m_interface->isValid())
    {
        qWarning() << "InputDevicesManager::setKeyboardLayout ignored because InputDevices facade is read-only without an active MauiMan D-Bus interface.";
        return;
    }

    if (this->keyboardLayout() == keyboardLayout)
        return;

    sync(QStringLiteral("setKeyboardLayout"), keyboardLayout);
    loadSettings();
}

void MauiMan::InputDevicesManager::setKeyboardModel(QString keyboardModel)
{
    if (!m_interface || !m_interface->isValid())
    {
        qWarning() << "InputDevicesManager::setKeyboardModel ignored because InputDevices facade is read-only without an active MauiMan D-Bus interface.";
        return;
    }

    if (this->keyboardModel() == keyboardModel)
        return;

    sync(QStringLiteral("setKeyboardModel"), keyboardModel);
    loadSettings();
}

void MauiMan::InputDevicesManager::setKeyboardVariant(QString keyboardVariant)
{
    if (!m_interface || !m_interface->isValid())
    {
        qWarning() << "InputDevicesManager::setKeyboardVariant ignored because InputDevices facade is read-only without an active MauiMan D-Bus interface.";
        return;
    }

    if (this->keyboardVariant() == keyboardVariant)
        return;

    sync(QStringLiteral("setKeyboardVariant"), keyboardVariant);
    loadSettings();
}

void MauiMan::InputDevicesManager::setKeyboardOptions(QString keyboardOptions)
{
    if (!m_interface || !m_interface->isValid())
    {
        qWarning() << "InputDevicesManager::setKeyboardOptions ignored because InputDevices facade is read-only without an active MauiMan D-Bus interface.";
        return;
    }

    if (this->keyboardOptions() == keyboardOptions)
        return;

    sync(QStringLiteral("setKeyboardOptions"), keyboardOptions);
    loadSettings();
}

void MauiMan::InputDevicesManager::setKeyboardRules(QString keyboardRules)
{
    if (!m_interface || !m_interface->isValid())
    {
        qWarning() << "InputDevicesManager::setKeyboardRules ignored because InputDevices facade is read-only without an active MauiMan D-Bus interface.";
        return;
    }

    if (this->keyboardRules() == keyboardRules)
        return;

    sync(QStringLiteral("setKeyboardRules"), keyboardRules);
    loadSettings();
}

void MauiMan::InputDevicesManager::onKeyboardLayoutChanged(const QString &keyboardLayout)
{
    Q_UNUSED(keyboardLayout);
    const QString previousKeyboardLayout = m_keyboardLayout;

    loadSettings();
    if (m_keyboardLayout == previousKeyboardLayout)
        return;

    Q_EMIT keyboardLayoutChanged(m_keyboardLayout);
}

void MauiMan::InputDevicesManager::onKeyboardModelChanged(const QString &keyboardModel)
{
    Q_UNUSED(keyboardModel);
    const QString previousKeyboardModel = m_keyboardModel;

    loadSettings();
    if (m_keyboardModel == previousKeyboardModel)
        return;

    Q_EMIT keyboardModelChanged(m_keyboardModel);
}

void MauiMan::InputDevicesManager::onKeyboardOptionsChanged(const QString &keyboardOptions)
{
    Q_UNUSED(keyboardOptions);
    const QString previousKeyboardOptions = m_keyboardOptions;

    loadSettings();
    if (m_keyboardOptions == previousKeyboardOptions)
        return;

    Q_EMIT keyboardOptionsChanged(m_keyboardOptions);
}

void MauiMan::InputDevicesManager::onKeyboardRulesChanged(const QString &keyboardRules)
{
    Q_UNUSED(keyboardRules);
    const QString previousKeyboardRules = m_keyboardRules;

    loadSettings();
    if (m_keyboardRules == previousKeyboardRules)
        return;

    Q_EMIT keyboardRulesChanged(m_keyboardRules);
}

void MauiMan::InputDevicesManager::onKeyboardVariantChanged(const QString &keyboardVariant)
{
    Q_UNUSED(keyboardVariant);
    const QString previousKeyboardVariant = m_keyboardVariant;

    loadSettings();
    if (m_keyboardVariant == previousKeyboardVariant)
        return;

    Q_EMIT keyboardVariantChanged(m_keyboardVariant);
}

void MauiMan::InputDevicesManager::sync(const QString &key, const QVariant &value)
{
    if (m_interface && m_interface->isValid())
    {
        const QDBusMessage reply = m_interface->call(key, value);
        if (reply.type() == QDBusMessage::ErrorMessage)
        {
            qWarning() << "InputDevicesManager::sync failed for call" << key << ":" << reply.errorMessage();
        }
        return;
    }

    qWarning() << "InputDevicesManager::sync skipped (no valid org.mauiman.InputDevices interface) for call:" << key;
}

void MauiMan::InputDevicesManager::setConnections()
{
    if(m_interface)
    {
        m_interface->disconnect();
        m_interface->deleteLater();
        m_interface = nullptr;
    }

    m_interface = new OrgMauimanInputDevicesInterface(QStringLiteral("org.mauiman.Manager"),
                                                      QStringLiteral("/InputDevices"),
                                                      QDBusConnection::sessionBus(),
                                                      this);
    if (m_interface->isValid())
    {
        connect(m_interface, &OrgMauimanInputDevicesInterface::keyboardLayoutChanged, this, &InputDevicesManager::onKeyboardLayoutChanged);
        connect(m_interface, &OrgMauimanInputDevicesInterface::keyboardModelChanged, this, &InputDevicesManager::onKeyboardModelChanged);
        connect(m_interface, &OrgMauimanInputDevicesInterface::keyboardRulesChanged, this, &InputDevicesManager::onKeyboardRulesChanged);
        connect(m_interface, &OrgMauimanInputDevicesInterface::keyboardOptionsChanged, this, &InputDevicesManager::onKeyboardOptionsChanged);
        connect(m_interface, &OrgMauimanInputDevicesInterface::keyboardVariantChanged, this, &InputDevicesManager::onKeyboardVariantChanged);
        loadSettings();
    }
}

void MauiMan::InputDevicesManager::loadSettings()
{
    if(m_interface && m_interface->isValid())
    {
        const QVariant keyboardLayout = m_interface->property("keyboardLayout");
        if (keyboardLayout.isValid())
        {
            m_keyboardLayout = keyboardLayout.toString();
        }

        const QVariant keyboardModel = m_interface->property("keyboardModel");
        if (keyboardModel.isValid())
        {
            m_keyboardModel = keyboardModel.toString();
        }

        const QVariant keyboardOptions = m_interface->property("keyboardOptions");
        if (keyboardOptions.isValid())
        {
            m_keyboardOptions = keyboardOptions.toString();
        }

        const QVariant keyboardRules = m_interface->property("keyboardRules");
        if (keyboardRules.isValid())
        {
            m_keyboardRules = keyboardRules.toString();
        }

        const QVariant keyboardVariant = m_interface->property("keyboardVariant");
        if (keyboardVariant.isValid())
        {
            m_keyboardVariant = keyboardVariant.toString();
        }
    }
}
