#pragma once

#include "FormFactorBase.h"
#include "modules/formfactormanager.h"

/**
 * @brief Exports form-factor preferences and detected device capabilities.
 *
 * Persisted preferences are supplied by the generated FormFactorBase. Runtime
 * capability values mirror MauiMan::FormFactorInfo and update as it changes.
 */
class FormFactor : public FormFactorBase
{
    Q_OBJECT
    /** The FormFactor D-Bus schema version used for feature detection. */
    Q_PROPERTY(uint version READ version CONSTANT)

    /** Runtime-recommended UI mode derived from display and input capabilities. */
    Q_PROPERTY(uint bestMode READ bestMode NOTIFY bestModeChanged)
    /** The default mode selected from the process environment. */
    Q_PROPERTY(uint defaultMode READ defaultMode CONSTANT)
    /** Whether a physical keyboard is currently detected. */
    Q_PROPERTY(bool hasKeyboard READ hasKeyboard NOTIFY hasKeyboardChanged)
    /** Whether a touchscreen is currently detected. */
    Q_PROPERTY(bool hasTouchscreen READ hasTouchscreen NOTIFY hasTouchscreenChanged)
    /** Whether a mouse is currently detected. */
    Q_PROPERTY(bool hasMouse READ hasMouse NOTIFY hasMouseChanged)
    /** Whether a touchpad is currently detected. */
    Q_PROPERTY(bool hasTouchpad READ hasTouchpad NOTIFY hasTouchpadChanged)

public:
    explicit FormFactor(QObject *parent = nullptr);

    uint version() const { return 1u; }

    uint bestMode() const { return m_bestMode; }
    uint defaultMode() const { return m_defaultMode; }
    bool hasKeyboard() const { return m_hasKeyboard; }
    bool hasTouchscreen() const { return m_hasTouchscreen; }
    bool hasMouse() const { return m_hasMouse; }
    bool hasTouchpad() const { return m_hasTouchpad; }

Q_SIGNALS:
    void bestModeChanged(uint bestMode);
    void hasKeyboardChanged(bool hasKeyboard);
    void hasTouchscreenChanged(bool hasTouchscreen);
    void hasMouseChanged(bool hasMouse);
    void hasTouchpadChanged(bool hasTouchpad);

private Q_SLOTS:
    void setBestMode(uint bestMode);
    void setHasKeyboard(bool value);
    void setHasMouse(bool value);
    void setHasTouchpad(bool value);
    void setHasTouchscreen(bool value);

private:
    MauiMan::FormFactorInfo *m_manager;
    uint m_bestMode = MauiMan::FormFactorManager::DefaultValues::defaultMode;
    uint m_defaultMode = MauiMan::FormFactorManager::DefaultValues::defaultMode;
    bool m_hasKeyboard = false;
    bool m_hasTouchscreen = false;
    bool m_hasMouse = false;
    bool m_hasTouchpad = false;
};
