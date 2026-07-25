include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}

    ${CMAKE_CURRENT_SOURCE_DIR}/modules)

set(mauiman_dbus_client_SRCS)
qt_add_dbus_interface(mauiman_dbus_client_SRCS
    ${CMAKE_SOURCE_DIR}/server/src/modules/org.mauiman.FormFactor.xml
    formfactor_interface)
qt_add_dbus_interface(mauiman_dbus_client_SRCS
    ${CMAKE_SOURCE_DIR}/server/src/modules/org.mauiman.Theme.xml
    theme_interface)
qt_add_dbus_interface(mauiman_dbus_client_SRCS
    ${CMAKE_SOURCE_DIR}/server/src/modules/org.mauiman.InputDevices.xml
    inputdevices_interface)

set(mauiman_HDRS
    settingsstore.h
    modules/thememanager.h
    modules/formfactormanager.h
    modules/inputdevicesmanager.h
    mauimanutils.h)

set(mauiman_SRCS
    settingsstore.cpp
    modules/thememanager.cpp
    modules/formfactormanager.cpp
    modules/inputdevicesmanager.cpp
    mauimanutils.cpp
    ${mauiman_dbus_client_SRCS})

add_library(${PROJECT_NAME}${MAUI_MAJOR_VERSION}
    SHARED
    ${mauiman_SRCS})

generate_export_header(${PROJECT_NAME}${MAUI_MAJOR_VERSION} BASE_NAME MauiMan)

target_include_directories(${PROJECT_NAME}${MAUI_MAJOR_VERSION}
    INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}>")

target_include_directories(${PROJECT_NAME}${MAUI_MAJOR_VERSION} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR};>")

target_link_libraries(${PROJECT_NAME}${MAUI_MAJOR_VERSION}
    PRIVATE
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Gui
    Qt${QT_MAJOR_VERSION}::DBus)

set_target_properties(${PROJECT_NAME}${MAUI_MAJOR_VERSION}
    PROPERTIES
    VERSION ${MAUIMAN_SO_VERSION}
    SOVERSION ${MAUIMAN_SO_VERSION})

install(FILES
    ${mauiman_HDRS}
    ${CMAKE_CURRENT_BINARY_DIR}/mauiman_export.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/MauiMan${MAUI_MAJOR_VERSION}
    COMPONENT Devel)

install(TARGETS ${PROJECT_NAME}${MAUI_MAJOR_VERSION} EXPORT MauiMan${MAUI_MAJOR_VERSION}Targets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

# QML plugin module: lets apps `import org.mauiman 1.0` and use the manager
# QObjects directly without having to qmlRegisterType in each app.
find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} CONFIG QUIET COMPONENTS Qml)
if(Qt${QT_MAJOR_VERSION}Qml_FOUND AND COMMAND ecm_add_qml_module)
    ecm_add_qml_module(MauiManQml URI "org.mauiman" VERSION 1.0
        GENERATE_PLUGIN_SOURCE)

    target_sources(MauiManQml PRIVATE
        qmlplugin.cpp)

    target_link_libraries(MauiManQml
        PRIVATE
        ${PROJECT_NAME}${MAUI_MAJOR_VERSION}
        Qt${QT_MAJOR_VERSION}::Qml)

    ecm_finalize_qml_module(MauiManQml DESTINATION ${KDE_INSTALL_QMLDIR})
endif()
