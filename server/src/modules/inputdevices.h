#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

#include "modules/inputdevicesmanager.h"

class QDBusInterface;

/**
 * @brief Exports current keyboard configuration through D-Bus.
 *
 * Initial values come from XKB_DEFAULT_* environment variables. When the KDE
 * keyboard-layout service is available, keyboardLayout follows its live state.
 * This server object is read-only and does not persist input preferences.
 */
class InputDevices : public QObject
{
    Q_OBJECT
    /** The active XKB layout name, such as us. */
    Q_PROPERTY(QString keyboardLayout READ keyboardLayout NOTIFY keyboardLayoutChanged)
    /** The active XKB keyboard model name. */
    Q_PROPERTY(QString keyboardModel READ keyboardModel NOTIFY keyboardModelChanged)
    /** The active XKB layout variant. */
    Q_PROPERTY(QString keyboardVariant READ keyboardVariant NOTIFY keyboardVariantChanged)
    /** The comma-separated active XKB options. */
    Q_PROPERTY(QString keyboardOptions READ keyboardOptions NOTIFY keyboardOptionsChanged)
    /** The active XKB ruleset name. */
    Q_PROPERTY(QString keyboardRules READ keyboardRules NOTIFY keyboardRulesChanged)

public:
    explicit InputDevices(QObject *parent = nullptr);

    QString keyboardLayout() const;
    QString keyboardModel() const;
    QString keyboardVariant() const;
    QString keyboardOptions() const;
    QString keyboardRules() const;

private Q_SLOTS:
    void onKdeLayoutChanged(uint index);
    void onKdeLayoutListChanged();

private:
    void setupKdeLayoutBridge();
    void refreshCurrentKdeLayout(int preferredIndex = -1);
    QString layoutFromIndex(int index) const;

    QString m_keyboardLayout = MauiMan::InputDevicesManager::DefaultValues::keyboardLayout;
    QString m_keyboardModel = MauiMan::InputDevicesManager::DefaultValues::keyboardModel;
    QString m_keyboardVariant = MauiMan::InputDevicesManager::DefaultValues::keyboardVariant;
    QString m_keyboardOptions = MauiMan::InputDevicesManager::DefaultValues::keyboardOptions;
    QString m_keyboardRules = MauiMan::InputDevicesManager::DefaultValues::keyboardRules;
    QStringList m_kdeLayoutShortNames;
    QDBusInterface *m_kdeLayoutsInterface = nullptr;
    bool m_missingKdeLayoutsWarned = false;

Q_SIGNALS:
    void keyboardLayoutChanged(QString keyboardLayout);
    void keyboardModelChanged(QString keyboardModel);
    void keyboardVariantChanged(QString keyboardVariant);
    void keyboardOptionsChanged(QString keyboardOptions);
    void keyboardRulesChanged(QString keyboardRules);
};
