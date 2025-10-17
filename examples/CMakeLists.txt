# Examples build configuration

# use-qtxdg example - demonstrates basic Qt6Xdg usage
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/use-qtxdg/CMakeLists.txt")
    add_subdirectory(use-qtxdg)
endif()

# qml-example - demonstrates QML plugin usage (only if QML plugin is built)
if(BUILD_QML_PLUGIN AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/qml-example/CMakeLists.txt")
    add_subdirectory(qml-example)
endif()
