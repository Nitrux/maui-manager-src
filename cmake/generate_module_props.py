#!/usr/bin/env python3
"""Generate property plumbing for a MauiMan persisted module from its D-Bus XML.

Reads the per-module XML (e.g. ``org.mauiman.Theme.xml``) and emits a header
declaring a ``<Module>Base`` QObject base class that contains:

  - the ``Q_PROPERTY`` lines for every readable property in the interface
  - inline getter/setter/signal definitions following the standard pattern
    ``if (m_x == v) return; m_x = v; savePref(...); Q_EMIT xChanged(m_x);``
  - member fields initialised from ``<Module>Manager::DefaultValues::<name>``
  - ``savePref()`` and ``loadAllPrefs()`` helpers that hit ``SettingsStore``
    via the standard begin/save/end pattern.

The concrete server-side class then subclasses ``<Module>Base`` and only has
to own its constructor (DBus adaptor registration + ``loadAllPrefs()``) and
``version()``. All the per-property boilerplate is generated.

Lib-side managers are out of scope for this pass — their pattern is more
varied (DBus-subscribe + local-fallback) and generating them benefits less.
"""

import argparse
import sys
import xml.etree.ElementTree as ET

# D-Bus type code -> (Q_PROPERTY type, setter argument type, QVariant accessor)
TYPE_MAP = {
    "s": ("QString", "const QString &", "toString"),
    "b": ("bool", "bool", "toBool"),
    "u": ("uint", "uint", "toUInt"),
    "i": ("int", "int", "toInt"),
    "d": ("double", "double", "toDouble"),
}

# Settings-key naming exceptions. The default rule is "capitalize first letter
# of the property name" (e.g. accentColor -> AccentColor). Add an entry here
# when the historical config key on disk diverges from that rule, so existing
# user configs keep working without migration.
SETTINGS_KEY_OVERRIDES = {
    ("Background", "wallpaperSource"): "Wallpaper",
}

# Default-value naming exceptions. The default rule is
# ``<Module>Manager::DefaultValues::<propertyName>``. Add an entry when the
# manager class's DefaultValues struct uses a different name (e.g. FormFactor's
# ``preferredMode`` is seeded from ``DefaultValues::defaultMode``).
DEFAULT_VALUE_OVERRIDES = {
    ("FormFactor", "preferredMode"): "defaultMode",
}

# Properties that should be skipped entirely (read-only, derived elsewhere).
# Use sparingly — facade modules are already excluded by not being passed in.
SKIP_PROPERTIES = {
    ("FormFactor", "bestMode"),
    ("FormFactor", "defaultMode"),
    ("FormFactor", "hasKeyboard"),
    ("FormFactor", "hasTouchscreen"),
    ("FormFactor", "hasMouse"),
    ("FormFactor", "hasTouchpad"),
}


def cap(s):
    return s[0].upper() + s[1:]


def settings_key(module, prop):
    return SETTINGS_KEY_OVERRIDES.get((module, prop), cap(prop))


def default_value_ref(module, prop):
    name = DEFAULT_VALUE_OVERRIDES.get((module, prop), prop)
    return f"MauiMan::{module}Manager::DefaultValues::{name}"


def parse_properties(xml_path, module):
    tree = ET.parse(xml_path)
    iface = tree.getroot().find("interface")
    if iface is None:
        sys.exit(f"{xml_path}: no <interface> element")

    properties = []
    for elem in iface.findall("property"):
        name = elem.get("name")
        if (module, name) in SKIP_PROPERTIES:
            continue
        if name == "version":
            # Schema version is a CONSTANT, owned by the concrete class.
            continue
        dbus_type = elem.get("type")
        if dbus_type not in TYPE_MAP:
            sys.exit(f"{xml_path}: unsupported D-Bus type '{dbus_type}' on property '{name}'")
        properties.append((name, dbus_type, *TYPE_MAP[dbus_type]))
    return properties


def emit(module, xml_path, out_path):
    properties = parse_properties(xml_path, module)

    L = []
    L.append(f"// AUTO-GENERATED FROM org.mauiman.{module}.xml. DO NOT EDIT.")
    L.append("// Regenerate by re-running CMake (or `cmake --build . --target {module}-props-gen`).".format(module=module.lower()))
    L.append("")
    L.append("#pragma once")
    L.append("")
    L.append("#include <QObject>")
    L.append("#include <QString>")
    L.append("#include <QVariant>")
    L.append("")
    L.append('#include "settingsstore.h"')
    L.append(f'#include "modules/{module.lower()}manager.h"')
    L.append("")
    L.append(f"class {module}Base : public QObject")
    L.append("{")
    L.append("    Q_OBJECT")
    for name, _, qt_type, _, _ in properties:
        L.append(
            f"    Q_PROPERTY({qt_type} {name} READ {name} "
            f"WRITE set{cap(name)} NOTIFY {name}Changed)"
        )
    L.append("")
    L.append("public:")
    L.append(f"    explicit {module}Base(QObject *parent = nullptr)")
    L.append("        : QObject(parent)")
    L.append("        , m_settings(new MauiMan::SettingsStore(this))")
    L.append("    {")
    L.append("    }")
    L.append("")
    for name, _, qt_type, setter_arg, _ in properties:
        return_type = f"const {qt_type} &" if qt_type == "QString" else qt_type
        L.append(f"    {return_type} {name}() const {{ return m_{name}; }}")
        L.append(f"    void set{cap(name)}({setter_arg} value)")
        L.append("    {")
        L.append(f"        if (m_{name} == value)")
        L.append("            return;")
        L.append(f"        m_{name} = value;")
        key = settings_key(module, name)
        L.append(f'        savePref(QStringLiteral("{key}"), QVariant::fromValue(m_{name}));')
        L.append(f"        Q_EMIT {name}Changed(m_{name});")
        L.append("    }")
        L.append("")
    L.append("Q_SIGNALS:")
    for name, _, qt_type, _, _ in properties:
        L.append(f"    void {name}Changed({qt_type} value);")
    L.append("")
    L.append("protected:")
    L.append("    void loadAllPrefs()")
    L.append("    {")
    L.append(f'        m_settings->beginModule(QStringLiteral("{module}"));')
    for name, _, _, _, qvariant_accessor in properties:
        key = settings_key(module, name)
        L.append(f'        m_{name} = m_settings->load(QStringLiteral("{key}"), m_{name}).{qvariant_accessor}();')
    L.append("        m_settings->endModule();")
    L.append("    }")
    L.append("")
    L.append("    void savePref(const QString &key, const QVariant &value)")
    L.append("    {")
    L.append(f'        m_settings->beginModule(QStringLiteral("{module}"));')
    L.append("        m_settings->save(key, value);")
    L.append("        m_settings->endModule();")
    L.append("    }")
    L.append("")
    L.append("    MauiMan::SettingsStore *m_settings;")
    L.append("")
    L.append("private:")
    for name, _, qt_type, _, _ in properties:
        L.append(f"    {qt_type} m_{name} = {default_value_ref(module, name)};")
    L.append("};")

    out_text = "\n".join(L) + "\n"

    # Skip writing if the content is unchanged so CMake doesn't see a phantom
    # rebuild every time.
    try:
        with open(out_path) as f:
            if f.read() == out_text:
                return
    except FileNotFoundError:
        pass

    with open(out_path, "w") as f:
        f.write(out_text)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--xml", required=True, help="Path to org.mauiman.<Module>.xml")
    ap.add_argument("--module", required=True, help="Module name, e.g. Theme")
    ap.add_argument("--output", required=True, help="Destination path for the generated header")
    args = ap.parse_args()
    emit(args.module, args.xml, args.output)


if __name__ == "__main__":
    main()
