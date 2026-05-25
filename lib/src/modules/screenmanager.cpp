#include "screenmanager.h"

#include "screen_interface.h"
#include "mauimanutils.h"

#include <QDebug>
#include <QDBusMessage>
#include <QGuiApplication>
#include <QScreen>
#include <QtGlobal>

namespace
{
bool hasPrimaryScreen()
{
    return QGuiApplication::instance() && QGuiApplication::primaryScreen();
}
}

using namespace MauiMan;

ScreenManager::ScreenManager(QObject * parent ) : QObject(parent)
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

double ScreenManager::scaleFactor() const
{
    if (m_interface && m_interface->isValid())
    {
        const QVariant value = m_interface->property("scaleFactor");
        if (value.isValid())
        {
            return value.toDouble();
        }
    }

    if (hasPrimaryScreen())
    {
        return QGuiApplication::primaryScreen()->devicePixelRatio();
    }

    return m_scaleFactor;
}

void ScreenManager::setScaleFactor(double scaleFactor)
{
    if (!m_interface || !m_interface->isValid())
    {
        qWarning() << "ScreenManager::setScaleFactor ignored because Screen facade is read-only without an active MauiMan D-Bus interface.";
        return;
    }

    if (qFuzzyCompare(1.0 + this->scaleFactor(), 1.0 + scaleFactor))
        return;

    sync(QStringLiteral("setScaleFactor"), scaleFactor);
    loadSettings();
}

uint ScreenManager::orientation() const
{
    if (m_interface && m_interface->isValid())
    {
        const QVariant value = m_interface->property("orientation");
        if (value.isValid())
        {
            return value.toUInt();
        }
    }

    if (hasPrimaryScreen())
    {
        return static_cast<uint>(QGuiApplication::primaryScreen()->orientation());
    }

    return m_orientation;
}

void ScreenManager::setOrientation(uint orientation)
{
    if (!m_interface || !m_interface->isValid())
    {
        qWarning() << "ScreenManager::setOrientation ignored because Screen facade is read-only without an active MauiMan D-Bus interface.";
        return;
    }

    if (this->orientation() == orientation)
        return;

    sync(QStringLiteral("setOrientation"), orientation);
    loadSettings();
}

void ScreenManager::onScaleFactorChanged(double scale)
{
    Q_UNUSED(scale);
    const double previousScaleFactor = m_scaleFactor;

    loadSettings();
    if (qFuzzyCompare(1.0 + m_scaleFactor, 1.0 + previousScaleFactor))
        return;

    Q_EMIT scaleFactorChanged(m_scaleFactor);
}

void ScreenManager::onOrientationChanged(uint orientation)
{
    Q_UNUSED(orientation);
    const uint previousOrientation = m_orientation;

    loadSettings();
    if (m_orientation == previousOrientation)
        return;

    Q_EMIT orientationChanged(m_orientation);
}

void ScreenManager::sync(const QString &key, const QVariant &value)
{
    if (m_interface && m_interface->isValid())
    {
        const QDBusMessage reply = m_interface->call(key, value);
        if (reply.type() == QDBusMessage::ErrorMessage)
        {
            qWarning() << "ScreenManager::sync failed for call" << key << ":" << reply.errorMessage();
        }
        return;
    }

    qWarning() << "ScreenManager::sync skipped (no valid org.mauiman.Screen interface) for call:" << key;
}

void ScreenManager::setConnections()
{
    if(m_interface)
    {
        m_interface->disconnect();
        m_interface->deleteLater();
        m_interface = nullptr;
    }

    m_interface = new OrgMauimanScreenInterface(QStringLiteral("org.mauiman.Manager"),
                                                QStringLiteral("/Screen"),
                                                QDBusConnection::sessionBus(),
                                                this);
    if (m_interface->isValid())
    {
        connect(m_interface, &OrgMauimanScreenInterface::scaleFactorChanged, this, &ScreenManager::onScaleFactorChanged);
        connect(m_interface, &OrgMauimanScreenInterface::orientationChanged, this, &ScreenManager::onOrientationChanged);
        loadSettings();
    }
}

void ScreenManager::loadSettings()
{
    if(m_interface && m_interface->isValid())
    {
        const QVariant scaleFactor = m_interface->property("scaleFactor");
        if (scaleFactor.isValid())
        {
            m_scaleFactor = scaleFactor.toDouble();
        }

        const QVariant orientation = m_interface->property("orientation");
        if (orientation.isValid())
        {
            m_orientation = orientation.toUInt();
        }

        return;
    }

    if (hasPrimaryScreen())
    {
        m_scaleFactor = QGuiApplication::primaryScreen()->devicePixelRatio();
        m_orientation = static_cast<uint>(QGuiApplication::primaryScreen()->orientation());
    }
}
