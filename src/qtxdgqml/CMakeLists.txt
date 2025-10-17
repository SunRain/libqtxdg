set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# Find Qt6 Qml and Quick components
find_package(Qt6 ${QT_MINIMUM_VERSION} CONFIG REQUIRED Qml Quick Concurrent)

set(QTXDGQML_LIBRARY_NAME "Qt6XdgQml")
set(QTXDGQML_FILE_NAME "qt6xdgqml")

# QML module URI
set(QML_MODULE_URI "org.lxqt.qtxdg")
set(QML_MODULE_VERSION 6.0)

# Convert URI dots to slashes for file path
string(REPLACE "." "/" QML_MODULE_PATH "${QML_MODULE_URI}")

# Source files
set(qtxdgqml_SOURCES
    qtxdgqmlplugin.cpp
    qtxdgqmlplugin.h
    xdgdirswrapper.cpp
    xdgdirswrapper.h
    xdgiconwrapper.cpp
    xdgiconwrapper.h
    xdgdesktopfilewrapper.cpp
    xdgdesktopfilewrapper.h
    xdgautostartwrapper.cpp
    xdgautostartwrapper.h
    xdgdefaultappswrapper.cpp
    xdgdefaultappswrapper.h
    xdgmimetypewrapper.cpp
    xdgmimetypewrapper.h
    xdgmimeappswrapper.cpp
    xdgmimeappswrapper.h
    xdgmenuwrapper.cpp
    xdgmenuwrapper.h
    xdgsimplelistmodel.cpp
    xdgsimplelistmodel.h
    xdgapplicationsmodel.cpp
    xdgapplicationsmodel.h
    xdgautostartmodel.cpp
    xdgautostartmodel.h
    xdgmenutreemodel.cpp
    xdgmenutreemodel.h
    xdgiconprovider.cpp
    xdgiconprovider.h
    xdgiconengineinitializer.cpp
    xdgiconengineinitializer.h
    fasticonprovider.cpp
    fasticonprovider.h
    fasticonresponse.cpp
    fasticonresponse.h
    fasticonstats.cpp
    fasticonstats.h
    cachedtexturefactory.cpp
    cachedtexturefactory.h
    diskiconcache.cpp
    diskiconcache.h
    iconusagetracker.cpp
    iconusagetracker.h
)

# Create QML module using qt_add_qml_module
qt_add_qml_module(${QTXDGQML_LIBRARY_NAME}
    URI ${QML_MODULE_URI}
    VERSION ${QML_MODULE_VERSION}
    SOURCES ${qtxdgqml_SOURCES}
    PLUGIN_TARGET ${QTXDGQML_LIBRARY_NAME}
    OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml/${QML_MODULE_PATH}"
    NO_PLUGIN_OPTIONAL
    NO_GENERATE_PLUGIN_SOURCE
)

# Link against required libraries
target_link_libraries(${QTXDGQML_LIBRARY_NAME}
    PUBLIC
        Qt6::Core
        Qt6::Qml
        Qt6::Quick
        Qt6::Concurrent
        ${QTXDGX_LIBRARY_NAME}
)

# Set target properties
set_target_properties(${QTXDGQML_LIBRARY_NAME} PROPERTIES
    VERSION ${QTXDG_VERSION_STRING}
    SOVERSION ${QTXDG_MAJOR_VERSION}
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml/${QML_MODULE_PATH}"
)

# Include directories
target_include_directories(${QTXDGQML_LIBRARY_NAME}
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src/qtxdg
)

# Install the QML module
install(TARGETS ${QTXDGQML_LIBRARY_NAME}
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}/qt6/qml/${QML_MODULE_PATH}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_LIBDIR}/qt6/qml/${QML_MODULE_PATH}"
    COMPONENT Runtime
)

# Install qmldir and other QML module files
install(FILES
    "${CMAKE_BINARY_DIR}/qml/${QML_MODULE_PATH}/qmldir"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/qt6/qml/${QML_MODULE_PATH}"
    COMPONENT Runtime
)

# Install typeinfo file if generated
if(EXISTS "${CMAKE_BINARY_DIR}/qml/${QML_MODULE_PATH}/plugins.qmltypes")
    install(FILES
        "${CMAKE_BINARY_DIR}/qml/${QML_MODULE_PATH}/plugins.qmltypes"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/qt6/qml/${QML_MODULE_PATH}"
        COMPONENT Runtime
    )
endif()

message(STATUS "QML module ${QML_MODULE_URI} configured")
message(STATUS "  Output directory: ${CMAKE_BINARY_DIR}/qml/${QML_MODULE_PATH}")
message(STATUS "  Install destination: ${CMAKE_INSTALL_LIBDIR}/qt6/qml/${QML_MODULE_PATH}")
