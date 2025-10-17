cmake_minimum_required(VERSION 3.18.0)
project(qtxdg-qml-example)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Qml QuickControls2)

# Add the QML module to import path for development
list(APPEND QML_IMPORT_PATH "${CMAKE_BINARY_DIR}/qml")

qt_add_executable(qtxdg-qml-example
    main.cpp
)

qt_add_qml_module(qtxdg-qml-example
    URI QtXdgQmlExample
    VERSION 1.0
    QML_FILES
        main.qml
        MimeTypeExample.qml
        ApplicationsExample.qml
        MenuTreeExample.qml
        ApplicationLauncher.qml
        SearchModesExample.qml
        PerformanceDashboard.qml
        IconCacheStressTest.qml
    RESOURCE_PREFIX /
)

target_link_libraries(qtxdg-qml-example PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Qml
    Qt6::QuickControls2
)

# Add compile definition for QML module path in development builds
target_compile_definitions(qtxdg-qml-example PRIVATE
    QML_MODULE_BUILD_DIR="${CMAKE_BINARY_DIR}/qml"
)

# Set the QML import path for finding our module
set_target_properties(qtxdg-qml-example PROPERTIES
    QML_IMPORT_PATH "${CMAKE_BINARY_DIR}/qml"
)

# Install target
install(TARGETS qtxdg-qml-example
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    COMPONENT Examples
)
