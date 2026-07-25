#include "settingsstore.h"
#include <QDir>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QSettings>
#include <QTimer>

using namespace MauiMan;
SettingsStore::SettingsStore(QObject *parent) : QObject(parent)
  ,m_settings(new QSettings(QStringLiteral("Maui"), QStringLiteral("MauiMan"), this))
  ,m_watcher(new QFileSystemWatcher(this))
  ,m_reloadTimer(new QTimer(this))
{
    // Native QSettings location for this org/app pair is ~/.config/Maui/MauiMan.conf.
    // MauiMan intentionally has no separate runtime defaults layer; defaults come
    // from code and any seeded per-user file (for example via /etc/skel).
    const QFileInfo configFile(m_settings->fileName());
    m_configPath = configFile.absoluteFilePath();
    m_configDirectory = configFile.absolutePath();

    // Watching the directory as well as the file keeps monitoring active when
    // editors save by atomically replacing MauiMan.conf.
    QDir().mkpath(m_configDirectory);
    refreshWatchPaths();

    m_reloadTimer->setInterval(50);
    m_reloadTimer->setSingleShot(true);

    connect(m_watcher, &QFileSystemWatcher::fileChanged, this, [this] {
        scheduleReload();
    });
    connect(m_watcher, &QFileSystemWatcher::directoryChanged, this, [this] {
        scheduleReload();
    });
    connect(m_reloadTimer, &QTimer::timeout, this, [this] {
        m_settings->sync();
        refreshWatchPaths();
        Q_EMIT settingsChanged();
    });
}

void SettingsStore::refreshWatchPaths()
{
    if (!m_watcher->directories().contains(m_configDirectory))
        m_watcher->addPath(m_configDirectory);

    if (QFileInfo::exists(m_configPath) && !m_watcher->files().contains(m_configPath))
        m_watcher->addPath(m_configPath);
}

void SettingsStore::scheduleReload()
{
    m_reloadTimer->start();
}

QVariant SettingsStore::load(const QString &key, const QVariant &defaultValue)
{
    return m_settings->value(key, defaultValue);
}

void SettingsStore::save(const QString &key, const QVariant &value)
{
    m_settings->setValue(key, value);
}

void SettingsStore::beginModule(const QString &module)
{
    m_settings->beginGroup(module);
}

void SettingsStore::endModule()
{
    m_settings->endGroup();
}
