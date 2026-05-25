#include "backgroundmanager.h"

#include "background_interface.h"
#include "settingsstore.h"
#include "mauimanutils.h"

#include <QDebug>
#include <QDBusPendingReply>

using namespace MauiMan;
BackgroundManager::BackgroundManager(QObject *parent) : QObject(parent)
  ,m_settings(new MauiMan::SettingsStore(this))
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

QString BackgroundManager::wallpaperSource() const
{
    return m_wallpaperSource;
}

bool BackgroundManager::dimWallpaper() const
{
    return m_dimWallpaper;
}

bool BackgroundManager::fitWallpaper() const
{
    return m_fitWallpaper;
}

QColor BackgroundManager::solidColor() const
{
    return m_solidColor;
}

bool BackgroundManager::showWallpaper() const
{
    return m_showWallpaper;
}

void BackgroundManager::setWallpaperSource(QString wallpaperSource)
{
    if (m_wallpaperSource == wallpaperSource)
        return;

    m_wallpaperSource = wallpaperSource;
    if (!sync(QStringLiteral("setWallpaperSource"), m_wallpaperSource))
    {
        // Persist locally only when DBus write fails/unavailable.
        m_settings->save(QStringLiteral("Wallpaper"), m_wallpaperSource);
    }
    Q_EMIT wallpaperSourceChanged(m_wallpaperSource);
}

void BackgroundManager::setDimWallpaper(bool dimWallpaper)
{
    if (m_dimWallpaper == dimWallpaper)
        return;

    m_dimWallpaper = dimWallpaper;
    if (!sync(QStringLiteral("setDimWallpaper"), m_dimWallpaper))
    {
        m_settings->save(QStringLiteral("DimWallpaper"), m_dimWallpaper);
    }
    Q_EMIT dimWallpaperChanged(m_dimWallpaper);
}

void BackgroundManager::setFitWallpaper(bool fitWallpaper)
{
    if (m_fitWallpaper == fitWallpaper)
        return;

    m_fitWallpaper = fitWallpaper;
    if (!sync(QStringLiteral("setFitWallpaper"), m_fitWallpaper))
    {
        m_settings->save(QStringLiteral("FitWallpaper"), m_fitWallpaper);
    }
    Q_EMIT fitWallpaperChanged(m_fitWallpaper);
}

void BackgroundManager::setSolidColor(const QColor &solidColor)
{
    if (m_solidColor == solidColor)
        return;

    m_solidColor = solidColor;
    if (!sync(QStringLiteral("setSolidColor"), m_solidColor.name()))
    {
        m_settings->save(QStringLiteral("SolidColor"), m_solidColor.name());
    }
    Q_EMIT solidColorChanged(m_solidColor);
}

void BackgroundManager::setShowWallpaper(bool showWallpaper)
{
    if (m_showWallpaper == showWallpaper)
        return;

    m_showWallpaper = showWallpaper;
    if (!sync(QStringLiteral("setShowWallpaper"), m_showWallpaper))
    {
        m_settings->save(QStringLiteral("ShowWallpaper"), m_showWallpaper);
    }
    Q_EMIT showWallpaperChanged(m_showWallpaper);
}

QString BackgroundManager::wallpaperSourceDir() const
{
    return m_wallpaperSourceDir;
}

void BackgroundManager::setWallpaperSourceDir(QString wallpaperSourceDir)
{
    if (m_wallpaperSourceDir == wallpaperSourceDir)
        return;

    m_wallpaperSourceDir = wallpaperSourceDir;
    Q_EMIT wallpaperSourceDirChanged(m_wallpaperSourceDir);
}

void BackgroundManager::onWallpaperChanged(const QString &wallpaperSource)
{
    if (m_wallpaperSource == wallpaperSource)
        return;

    m_wallpaperSource = wallpaperSource;
    Q_EMIT wallpaperSourceChanged(m_wallpaperSource);
}

void BackgroundManager::onSolidColorChanged(const QString &solidColor)
{
    const QColor incoming(solidColor);
    if (m_solidColor == incoming)
        return;

    m_solidColor = incoming;
    Q_EMIT solidColorChanged(m_solidColor);
}

void BackgroundManager::onFitWallpaperChanged(const bool &fitWallpaper)
{
    if (m_fitWallpaper == fitWallpaper)
        return;

    m_fitWallpaper = fitWallpaper;
    Q_EMIT fitWallpaperChanged(m_fitWallpaper);
}

void BackgroundManager::onDimWallpaperChanged(const bool &dimWallpaper)
{
    if (m_dimWallpaper == dimWallpaper)
        return;

    m_dimWallpaper = dimWallpaper;
    Q_EMIT dimWallpaperChanged(m_dimWallpaper);
}

void BackgroundManager::onShowWallpaperChanged(const bool &showWallpaper)
{
    if (m_showWallpaper == showWallpaper)
        return;

    m_showWallpaper = showWallpaper;
    Q_EMIT showWallpaperChanged(m_showWallpaper);
}

bool BackgroundManager::sync(const QString &key, const QVariant &value)
{
    if (m_interface && m_interface->isValid())
    {
        auto finishCall = [key](QDBusPendingReply<> reply) {
            reply.waitForFinished();
            if (!reply.isError())
            {
                return true;
            }

            qWarning() << "BackgroundManager::sync failed for call" << key << ":" << reply.error().message();
            return false;
        };

        if (key == QStringLiteral("setWallpaperSource"))
        {
            return finishCall(m_interface->setWallpaperSource(value.toString()));
        } else if (key == QStringLiteral("setDimWallpaper"))
        {
            return finishCall(m_interface->setDimWallpaper(value.toBool()));
        } else if (key == QStringLiteral("setFitWallpaper"))
        {
            return finishCall(m_interface->setFitWallpaper(value.toBool()));
        } else if (key == QStringLiteral("setSolidColor"))
        {
            return finishCall(m_interface->setSolidColor(value.toString()));
        } else if (key == QStringLiteral("setShowWallpaper"))
        {
            return finishCall(m_interface->setShowWallpaper(value.toBool()));
        } else
        {
            qWarning() << "BackgroundManager::sync received unknown method key:" << key;
            return false;
        }
    }

    return false;
}

void BackgroundManager::setConnections()
{
    if(m_interface)
    {
        m_interface->disconnect();
        m_interface->deleteLater();
        m_interface = nullptr;
    }

    m_interface = new OrgMauimanBackgroundInterface(QStringLiteral("org.mauiman.Manager"),
                                                    QStringLiteral("/Background"),
                                                    QDBusConnection::sessionBus(),
                                                    this);
    if (m_interface->isValid())
    {
        connect(m_interface, &OrgMauimanBackgroundInterface::wallpaperSourceChanged, this, &BackgroundManager::onWallpaperChanged);
        connect(m_interface, &OrgMauimanBackgroundInterface::solidColorChanged, this, &BackgroundManager::onSolidColorChanged);
        connect(m_interface, &OrgMauimanBackgroundInterface::fitWallpaperChanged, this, &BackgroundManager::onFitWallpaperChanged);
        connect(m_interface, &OrgMauimanBackgroundInterface::showWallpaperChanged, this, &BackgroundManager::onShowWallpaperChanged);
        connect(m_interface, &OrgMauimanBackgroundInterface::dimWallpaperChanged, this, &BackgroundManager::onDimWallpaperChanged);

    }
}

void BackgroundManager::loadSettings()
{
    m_settings->beginModule(QStringLiteral("Background"));

    if(m_interface && m_interface->isValid())
    {
        // When server is available, DBus properties are the source of truth.
        m_wallpaperSource = m_interface->property("wallpaperSource").toString();
        m_dimWallpaper = m_interface->property("dimWallpaper").toBool();
        m_showWallpaper = m_interface->property("showWallpaper").toBool();
        m_fitWallpaper = m_interface->property("fitWallpaper").toBool();
        m_solidColor = QColor(m_interface->property("solidColor").toString());
        m_settings->endModule();
        return;
    }

    // Offline fallback: use cached local settings until DBus service appears.
    m_wallpaperSource = m_settings->load(QStringLiteral("Wallpaper"), m_wallpaperSource).toString();
    m_dimWallpaper = m_settings->load(QStringLiteral("DimWallpaper"), m_dimWallpaper).toBool();
    m_showWallpaper = m_settings->load(QStringLiteral("ShowWallpaper"), m_showWallpaper).toBool();
    m_fitWallpaper = m_settings->load(QStringLiteral("FitWallpaper"), m_fitWallpaper).toBool();
    m_solidColor = QColor(m_settings->load(QStringLiteral("SolidColor"), m_solidColor.name()).toString());
    m_settings->endModule();
}
