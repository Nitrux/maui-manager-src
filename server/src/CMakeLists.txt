include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR})

find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(MAUIMAN_GENERATOR ${CMAKE_SOURCE_DIR}/cmake/generate_module_props.py)

# MauiKit preference modules whose Q_PROPERTY/getter/setter/signal/member
# plumbing is generated from their D-Bus XML.
set(MAUIMAN_PERSISTED_MODULES Theme FormFactor)
foreach(MODULE ${MAUIMAN_PERSISTED_MODULES})
    set(MODULE_XML ${CMAKE_CURRENT_SOURCE_DIR}/modules/org.mauiman.${MODULE}.xml)
    set(MODULE_GEN ${CMAKE_CURRENT_BINARY_DIR}/${MODULE}Base.h)
    add_custom_command(
        OUTPUT ${MODULE_GEN}
        COMMAND ${Python3_EXECUTABLE} ${MAUIMAN_GENERATOR}
                --xml ${MODULE_XML}
                --module ${MODULE}
                --output ${MODULE_GEN}
        DEPENDS ${MODULE_XML} ${MAUIMAN_GENERATOR}
        COMMENT "Generating ${MODULE}Base.h from org.mauiman.${MODULE}.xml"
        VERBATIM)
    list(APPEND MAUIMAN_GENERATED_HEADERS ${MODULE_GEN})
endforeach()

set(mauimanserver_SRCS
    main.cpp
    server.cpp
    modules/theme.cpp
    modules/formfactor.cpp
    modules/inputdevices.cpp)

qt_add_dbus_adaptor(DBUS_SOURCES modules/org.mauiman.Theme.xml modules/theme.h Theme)
qt_add_dbus_adaptor(DBUS_SOURCES modules/org.mauiman.FormFactor.xml modules/formfactor.h FormFactor)
qt_add_dbus_adaptor(DBUS_SOURCES modules/org.mauiman.InputDevices.xml modules/inputdevices.h InputDevices)

set_source_files_properties(${DBUS_SOURCES} PROPERTIES SKIP_AUTOGEN ON)

add_executable(MauiManServer
    ${mauimanserver_SRCS}
    ${DBUS_SOURCES}
    ${MAUIMAN_GENERATED_HEADERS})

target_include_directories(MauiManServer
    INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}>")

target_include_directories(MauiManServer PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR};>")

target_link_libraries(MauiManServer
    PRIVATE
    MauiMan${MAUI_MAJOR_VERSION}
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Gui
    Qt${QT_MAJOR_VERSION}::DBus)

install(TARGETS MauiManServer ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
