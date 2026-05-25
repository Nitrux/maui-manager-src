#pragma once

#include "FormFactorBase.h"
#include "modules/formfactormanager.h"

class FormFactor : public FormFactorBase
{
    Q_OBJECT
    Q_PROPERTY(uint version READ version CONSTANT)

    // Read-only capabilities derived at runtime by MauiMan::FormFactorInfo;
    // not in the generated base because they aren't persisted.
    Q_PROPERTY(uint bestMode READ bestMode NOTIFY bestModeChanged)
    Q_PROPERTY(uint defaultMode READ defaultMode CONSTANT)
    Q_PROPERTY(bool hasKeyboard READ hasKeyboard NOTIFY hasKeyboardChanged)
    Q_PROPERTY(bool hasTouchscreen READ hasTouchscreen NOTIFY hasTouchscreenChanged)
    Q_PROPERTY(bool hasMouse READ hasMouse NOTIFY hasMouseChanged)
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
