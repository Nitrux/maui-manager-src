project('MauiMan', 'cpp', version: '4.0.5', meson_version: '>=1.12', default_options: ['cpp_std=c++17', 'buildtype=release'])

cpp_compiler = meson.get_compiler('cpp')
if cpp_compiler.get_id() == 'gcc' and cpp_compiler.has_argument('-Wno-sfinae-incomplete')
    add_project_arguments('-Wno-sfinae-incomplete', language: 'cpp')
endif

qt6 = import('qt6')
pkg = import('pkgconfig')
qt = dependency('qt6', version: '>=6.9', modules: ['Core', 'Gui', 'DBus', 'Qml'])
cpp_args = []
if host_machine.cpu_family() == 'x86_64'
    cpp_args += ['-march=x86-64-v3']
endif

qdbusxml2cpp = find_program('qdbusxml2cpp', dirs: ['/usr/lib/qt6/bin'])
qdbus_interface = custom_target(
    'mauiman-client-dbus',
    input: [
        'server/src/modules/org.mauiman.FormFactor.xml',
        'server/src/modules/org.mauiman.Theme.xml',
        'server/src/modules/org.mauiman.InputDevices.xml',
    ],
    output: ['formfactor_interface.cpp', 'formfactor_interface.h', 'theme_interface.cpp', 'theme_interface.h', 'inputdevices_interface.cpp', 'inputdevices_interface.h'],
    command: [
        'sh', '-c',
        'qdbusxml2cpp -p formfactor_interface @INPUT0@ && qdbusxml2cpp -p theme_interface @INPUT1@ && qdbusxml2cpp -p inputdevices_interface @INPUT2@',
    ],
)

mauiman_export = configure_file(
    input: 'meson-content.in',
    output: 'mauiman_export.h',
    configuration: {'CONTENT': ('#pragma once\n#define MAUIMAN_EXPORT __attribute__((visibility("default")))\n')}
)
lib_sources = [
    'lib/src/settingsstore.cpp',
    'lib/src/modules/thememanager.cpp',
    'lib/src/modules/formfactormanager.cpp',
    'lib/src/modules/inputdevicesmanager.cpp',
    'lib/src/mauimanutils.cpp',
    qdbus_interface,
]
lib_moc = qt6.preprocess(moc_headers: [
    'lib/src/settingsstore.h',
    'lib/src/modules/thememanager.h',
    'lib/src/modules/formfactormanager.h',
    'lib/src/modules/inputdevicesmanager.h',
    'lib/src/mauimanutils.h',
    qdbus_interface[1],
    qdbus_interface[3],
    qdbus_interface[5],
])
mauiman = shared_library(
    'MauiMan4',
    lib_sources + lib_moc,
    dependencies: [qt],
    include_directories: include_directories('lib/src'),
    cpp_args: cpp_args,
    version: '4.0.5',
    soversion: '4.0.5',
    install: true,
)
install_headers([
    'lib/src/settingsstore.h',
    'lib/src/modules/thememanager.h',
    'lib/src/modules/formfactormanager.h',
    'lib/src/modules/inputdevicesmanager.h',
    'lib/src/mauimanutils.h',
    mauiman_export,
], subdir: 'MauiMan4')

python = find_program('python3')
generator_script = files('cmake/generate_module_props.py')
theme_base = custom_target('theme-base', input: 'server/src/modules/org.mauiman.Theme.xml', output: 'ThemeBase.h', command: [python, generator_script, '--xml', '@INPUT@', '--module', 'Theme', '--output', '@OUTPUT@'])
formfactor_base = custom_target('formfactor-base', input: 'server/src/modules/org.mauiman.FormFactor.xml', output: 'FormFactorBase.h', command: [python, generator_script, '--xml', '@INPUT@', '--module', 'FormFactor', '--output', '@OUTPUT@'])
server_adaptors = custom_target(
    'mauiman-server-dbus',
    input: ['server/src/modules/org.mauiman.Theme.xml', 'server/src/modules/org.mauiman.FormFactor.xml', 'server/src/modules/org.mauiman.InputDevices.xml'],
    output: ['themeadaptor.cpp', 'themeadaptor.h', 'formfactoradaptor.cpp', 'formfactoradaptor.h', 'inputdevicesadaptor.cpp', 'inputdevicesadaptor.h'],
    command: ['sh', '-c', 'qdbusxml2cpp -a themeadaptor @INPUT0@ && qdbusxml2cpp -a formfactoradaptor @INPUT1@ && qdbusxml2cpp -a inputdevicesadaptor @INPUT2@'],
)
server_moc = qt6.preprocess(moc_headers: [
    theme_base,
    formfactor_base,
    'server/src/server.h',
    'server/src/modules/theme.h',
    'server/src/modules/formfactor.h',
    'server/src/modules/inputdevices.h',
    server_adaptors[1],
    server_adaptors[3],
    server_adaptors[5],
])
server = executable(
    'MauiManServer',
    ['server/src/main.cpp', 'server/src/server.cpp', 'server/src/modules/theme.cpp', 'server/src/modules/formfactor.cpp', 'server/src/modules/inputdevices.cpp', theme_base, formfactor_base, server_adaptors] + server_moc,
    dependencies: [qt],
    link_with: mauiman,
    include_directories: include_directories('lib/src', 'server/src'),
    cpp_args: cpp_args,
    install: true,
)

service = configure_file(input: 'server/org.mauiman.Manager.service.in.6', output: 'org.mauiman.Manager.service', configuration: {'CMAKE_INSTALL_PREFIX': '/usr'})
install_data(service, install_dir: get_option('datadir') / 'dbus-1' / 'services')

pkg.generate(
    mauiman,
    name: 'MauiMan4',
    filebase: 'MauiMan4',
    version: meson.project_version(),
    description: 'MauiMan system settings manager',
    requires: 'Qt6Core Qt6Gui Qt6DBus',
)

cmake_targets = configure_file(
    input: 'meson-content.in',
    output: 'MauiMan4Targets.cmake',
    configuration: {'CONTENT': (('''get_filename_component(_MAUIMAN_PREFIX "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
if(NOT TARGET MauiMan4)
  add_library(MauiMan4 SHARED IMPORTED)
  set_target_properties(MauiMan4 PROPERTIES
    IMPORTED_LOCATION "${_MAUIMAN_PREFIX}/@LIBDIR@/libMauiMan4.so.4.0.5"
    INTERFACE_INCLUDE_DIRECTORIES "${_MAUIMAN_PREFIX}/include")
endif()
if(NOT TARGET MauiMan4::MauiMan4)
  add_library(MauiMan4::MauiMan4 ALIAS MauiMan4)
endif()
''')).replace('@LIBDIR@', (get_option('libdir')))}
)
cmake_config = configure_file(
    input: 'meson-content.in',
    output: 'MauiMan4Config.cmake',
    configuration: {'CONTENT': ('''include(CMakeFindDependencyMacro)
find_dependency(Qt6 6.9 COMPONENTS Core Gui DBus)
include("${CMAKE_CURRENT_LIST_DIR}/MauiMan4Targets.cmake")
set(MauiMan4_FOUND TRUE)
''')}
)
cmake_version = configure_file(
    input: 'meson-content.in',
    output: 'MauiMan4ConfigVersion.cmake',
    configuration: {'CONTENT': (('''set(PACKAGE_VERSION "@VERSION@")
if(PACKAGE_FIND_VERSION VERSION_LESS_EQUAL PACKAGE_VERSION)
  set(PACKAGE_VERSION_COMPATIBLE TRUE)
endif()
if(PACKAGE_FIND_VERSION VERSION_EQUAL PACKAGE_VERSION)
  set(PACKAGE_VERSION_EXACT TRUE)
endif()
''')).replace('@VERSION@', (meson.project_version()))}
)
install_data([cmake_targets, cmake_config, cmake_version], install_dir: get_option('libdir') / 'cmake' / 'MauiMan4')

mauiman_qml_plugin_source = configure_file(
    input: 'meson-content.in',
    output: 'mauiman_qml_plugin.cpp',
    configuration: {'CONTENT': ('''#include <QQmlEngine>
#include <QQmlExtensionPlugin>
#include <qqml.h>
#include "modules/formfactormanager.h"
#include "modules/inputdevicesmanager.h"
#include "modules/thememanager.h"

class MauiManQmlPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
    void registerTypes(const char *uri) override
    {
        qmlRegisterSingletonType<MauiMan::ThemeManager>(uri, 1, 0, "Theme", [](QQmlEngine *engine, QJSEngine *) {
            return new MauiMan::ThemeManager(engine);
        });
        qmlRegisterSingletonType<MauiMan::FormFactorManager>(uri, 1, 0, "FormFactor", [](QQmlEngine *engine, QJSEngine *) {
            return new MauiMan::FormFactorManager(engine);
        });
        qmlRegisterSingletonType<MauiMan::InputDevicesManager>(uri, 1, 0, "InputDevices", [](QQmlEngine *engine, QJSEngine *) {
            return new MauiMan::InputDevicesManager(engine);
        });
    }
};
''')}
)
mauiman_qml_moc = qt6.preprocess(moc_sources: [mauiman_qml_plugin_source])
qml_plugin = shared_module('MauiManQml', [mauiman_qml_plugin_source] + mauiman_qml_moc, link_with: mauiman, dependencies: [qt], include_directories: include_directories('lib/src'), cpp_args: cpp_args, install: true, install_dir: get_option('libdir') / 'qt6/qml/org/mauiman')
qml_dir = get_option('libdir') / 'qt6/qml/org/mauiman'
install_data(configure_file(
    input: 'meson-content.in',
    output: 'mauiman_qmldir',
    configuration: {'CONTENT': ('''module org.mauiman
plugin MauiManQml
''')}
), install_dir: qml_dir, rename: 'qmldir')
