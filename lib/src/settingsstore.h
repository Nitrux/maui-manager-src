#pragma once
#include <QObject>
#include <QString>
#include <QVariant>
#include "mauiman_export.h"

class QSettings;
class QFileSystemWatcher;
class QTimer;

namespace MauiMan
{

/**
 * @brief Reads, writes, and monitors MauiMan preferences in the user configuration.
 *
 * Values are stored through QSettings in Maui/MauiMan.conf. beginModule() and
 * endModule() delimit a QSettings group. External file replacements are
 * detected and coalesced before settingsChanged() is emitted.
 */
class MAUIMAN_EXPORT SettingsStore : public QObject
{
    Q_OBJECT
public:

    /** Creates a store and begins monitoring its configuration file. */
    explicit SettingsStore(QObject *parent = nullptr);

    /** Returns the value for key, or defaultValue when the key is absent. */
    QVariant load(const QString &key, const QVariant &defaultValue);

    /** Stores value under key in the current module group. */
    void save(const QString &key, const QVariant &value);

    /** Enters the settings group for module. Calls may be nested. */
    void beginModule(const QString &module);

    /** Leaves the most recently entered module group. */
    void endModule();

Q_SIGNALS:
    /** Emitted after an external settings-file change has been resynchronized. */
    void settingsChanged();

private:
    void refreshWatchPaths();
    void scheduleReload();

    QSettings *m_settings;
    QFileSystemWatcher *m_watcher;
    QTimer *m_reloadTimer;
    QString m_configPath;
    QString m_configDirectory;
};
}
