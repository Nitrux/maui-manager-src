#pragma once

#include "ThemeBase.h"

/**
 * @brief Exports persisted MauiMan theme preferences on the session bus.
 *
 * The generated ThemeBase supplies the preference properties and persistence;
 * this wrapper registers them at the org.mauiman.Theme D-Bus object.
 */
class Theme : public ThemeBase
{
    Q_OBJECT
    /** The Theme D-Bus schema version used for feature detection. */
    Q_PROPERTY(uint version READ version CONSTANT)

public:
    explicit Theme(QObject *parent = nullptr);

    uint version() const { return 2u; }
};
