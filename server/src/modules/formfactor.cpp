#include "formfactor.h"
#include "formfactoradaptor.h"

#include <QDBusConnection>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(LOG_FORMFACTOR_SERVER, "mauiman.server.formfactor")

FormFactor::FormFactor(QObject *parent)
    : FormFactorBase(parent)
    , m_manager(new MauiMan::FormFactorInfo(this))
{
    qCInfo(LOG_FORMFACTOR_SERVER, "INIT FORMFACTOR MODULE");
    new FormFactorAdaptor(this);
    if (!QDBusConnection::sessionBus().registerObject(QStringLiteral("/FormFactor"), this))
    {
        qCWarning(LOG_FORMFACTOR_SERVER) << "FAILED TO REGISTER FORMFACTOR DBUS OBJECT";
        return;
    }

    m_defaultMode = m_manager->defaultMode();
    m_bestMode = m_manager->bestMode();
    m_hasKeyboard = m_manager->hasKeyboard();
    m_hasMouse = m_manager->hasMouse();
    m_hasTouchpad = m_manager->hasTouchpad();
    m_hasTouchscreen = m_manager->hasTouchscreen();

    connect(m_manager, &MauiMan::FormFactorInfo::bestModeChanged, this, &FormFactor::setBestMode);
    connect(m_manager, &MauiMan::FormFactorInfo::hasKeyboardChanged, this, &FormFactor::setHasKeyboard);
    connect(m_manager, &MauiMan::FormFactorInfo::hasMouseChanged, this, &FormFactor::setHasMouse);
    connect(m_manager, &MauiMan::FormFactorInfo::hasTouchpadChanged, this, &FormFactor::setHasTouchpad);
    connect(m_manager, &MauiMan::FormFactorInfo::hasTouchscreenChanged, this, &FormFactor::setHasTouchscreen);

    loadAllPrefs();
}

void FormFactor::setBestMode(uint bestMode)
{
    if (m_bestMode == bestMode)
        return;
    m_bestMode = bestMode;
    Q_EMIT bestModeChanged(m_bestMode);
}

void FormFactor::setHasKeyboard(bool value)
{
    if (m_hasKeyboard == value)
        return;
    m_hasKeyboard = value;
    Q_EMIT hasKeyboardChanged(m_hasKeyboard);
}

void FormFactor::setHasMouse(bool value)
{
    if (m_hasMouse == value)
        return;
    m_hasMouse = value;
    Q_EMIT hasMouseChanged(m_hasMouse);
}

void FormFactor::setHasTouchpad(bool value)
{
    if (m_hasTouchpad == value)
        return;
    m_hasTouchpad = value;
    Q_EMIT hasTouchpadChanged(m_hasTouchpad);
}

void FormFactor::setHasTouchscreen(bool value)
{
    if (m_hasTouchscreen == value)
        return;
    m_hasTouchscreen = value;
    Q_EMIT hasTouchscreenChanged(m_hasTouchscreen);
}
