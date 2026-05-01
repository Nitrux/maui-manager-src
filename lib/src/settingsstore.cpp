#include "settingsstore.h"
#include <QSettings>

using namespace MauiMan;
SettingsStore::SettingsStore(QObject *parent) : QObject(parent)
  ,m_settings(new QSettings(QStringLiteral("Maui"), QStringLiteral("MauiMan"), this))
{
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
