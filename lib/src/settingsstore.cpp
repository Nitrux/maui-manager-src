#include "settingsstore.h"
#include <QSettings>

using namespace MauiMan;
SettingsStore::SettingsStore(QObject *parent) : QObject(parent)
  ,m_settings(new QSettings(QStringLiteral("Maui"), QStringLiteral("MauiMan"), this))
{
    // Native QSettings location for this org/app pair is ~/.config/Maui/MauiMan.conf.
    // MauiMan intentionally has no separate runtime defaults layer; defaults come
    // from code and any seeded per-user file (for example via /etc/skel).
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
