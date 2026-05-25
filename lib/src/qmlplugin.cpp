// QML registration for the org.mauiman module. Uses QML_FOREIGN so the
// MauiMan*Manager classes don't need QML annotations in their public headers.
// qmltyperegistrar reads the macros below and the auto-generated plugin source
// (from ecm_add_qml_module) wires them up.

#include <QtQml/qqml.h>

#include "modules/accessibilitymanager.h"
#include "modules/backgroundmanager.h"
#include "modules/formfactormanager.h"
#include "modules/inputdevicesmanager.h"
#include "modules/screenmanager.h"
#include "modules/thememanager.h"

struct MauiManThemeQml
{
    Q_GADGET
    QML_FOREIGN(MauiMan::ThemeManager)
    QML_SINGLETON
    QML_NAMED_ELEMENT(Theme)
public:
    static MauiMan::ThemeManager *create(QQmlEngine *engine, QJSEngine *) { return new MauiMan::ThemeManager(engine); }
};

struct MauiManBackgroundQml
{
    Q_GADGET
    QML_FOREIGN(MauiMan::BackgroundManager)
    QML_SINGLETON
    QML_NAMED_ELEMENT(Background)
public:
    static MauiMan::BackgroundManager *create(QQmlEngine *engine, QJSEngine *) { return new MauiMan::BackgroundManager(engine); }
};

struct MauiManFormFactorQml
{
    Q_GADGET
    QML_FOREIGN(MauiMan::FormFactorManager)
    QML_SINGLETON
    QML_NAMED_ELEMENT(FormFactor)
public:
    static MauiMan::FormFactorManager *create(QQmlEngine *engine, QJSEngine *) { return new MauiMan::FormFactorManager(engine); }
};

struct MauiManScreenQml
{
    Q_GADGET
    QML_FOREIGN(MauiMan::ScreenManager)
    QML_SINGLETON
    QML_NAMED_ELEMENT(Screen)
public:
    static MauiMan::ScreenManager *create(QQmlEngine *engine, QJSEngine *) { return new MauiMan::ScreenManager(engine); }
};

struct MauiManAccessibilityQml
{
    Q_GADGET
    QML_FOREIGN(MauiMan::AccessibilityManager)
    QML_SINGLETON
    QML_NAMED_ELEMENT(Accessibility)
public:
    static MauiMan::AccessibilityManager *create(QQmlEngine *engine, QJSEngine *) { return new MauiMan::AccessibilityManager(engine); }
};

struct MauiManInputDevicesQml
{
    Q_GADGET
    QML_FOREIGN(MauiMan::InputDevicesManager)
    QML_SINGLETON
    QML_NAMED_ELEMENT(InputDevices)
public:
    static MauiMan::InputDevicesManager *create(QQmlEngine *engine, QJSEngine *) { return new MauiMan::InputDevicesManager(engine); }
};
