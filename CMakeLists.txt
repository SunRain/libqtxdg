cmake_minimum_required(VERSION 3.18.0 FATAL_ERROR)
# CMP0000: Call the cmake_minimum_required() command at the beginning of the top-level
# CMakeLists.txt file even before calling the project() command.
# The cmake_minimum_required(VERSION) command implicitly invokes the cmake_policy(VERSION)
# command to specify that the current project code is written for the given range of CMake
# versions.
project(libqtxdg)

option(BUILD_TESTS "Builds tests" OFF)
option(BUILD_DEV_UTILS "Builds and install development utils" OFF)
option(BUILD_EXAMPLES "Builds examples" ON)
option(BUILD_QML_PLUGIN "Build QML plugin for Qt6Xdg" ON)
option(BUILD_WIDGETS_LIB "Build Qt Widgets components (XdgAction, XdgMenuWidget)" ON)
option(QTXDG_INSTALL_DEFAPPS_CONFIG "Install default app config files" ON)

# additional cmake files
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(QTXDG_MAJOR_VERSION 4)
set(QTXDG_MINOR_VERSION 3)
set(QTXDG_PATCH_VERSION 0)
set(QTXDG_VERSION_STRING ${QTXDG_MAJOR_VERSION}.${QTXDG_MINOR_VERSION}.${QTXDG_PATCH_VERSION})

set(QT_MINIMUM_VERSION "6.6.0")
set(GLIB_MINIMUM_VERSION "2.41.0") # Mime Apps new implementation

find_package(Qt6 ${QT_MINIMUM_VERSION} CONFIG REQUIRED Svg Xml DBus)
find_package(GLIB ${GLIB_MINIMUM_VERSION} REQUIRED COMPONENTS gobject gio gio-unix)
#find_package(XTerm)

if(BUILD_WIDGETS_LIB)
    find_package(Qt6 ${QT_MINIMUM_VERSION} CONFIG REQUIRED Widgets)
endif()

if(BUILD_QML_PLUGIN)
    find_package(Qt6 ${QT_MINIMUM_VERSION} CONFIG REQUIRED Qml Quick)
endif()

if (Qt6Gui_VERSION VERSION_GREATER_EQUAL "6.10.0")
    find_package(Qt6GuiPrivate ${QT_MINIMUM_VERSION} REQUIRED)
endif()

include(GNUInstallDirs)             # Standard directories for installation
include(CMakePackageConfigHelpers)
include(GenerateExportHeader)

# 移除 LXQt 依赖，使用项目自主实现
# include(LXQtConfigVars) - 已使用 CMAKE_INSTALL_SYSCONFDIR 替代
# include(LXQtPreventInSourceBuilds) - 使用标准 CMake 检查替代

# 防止源码内构建
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed. "
        "Please create a separate build directory and run cmake from there:\n"
        "  mkdir build && cd build && cmake ..")
endif()

# 自定义函数：创建 Qt 风格便携头文件
# 用法: qtxdg_create_portable_headers(OUTPUT_VAR HEADER_NAMES <name1> <name2> ... OUTPUT_DIR <dir>)
function(qtxdg_create_portable_headers outfiles)
    cmake_parse_arguments(ARGS "" "OUTPUT_DIR" "HEADER_NAMES" ${ARGN})

    if(NOT ARGS_HEADER_NAMES)
        message(FATAL_ERROR "qtxdg_create_portable_headers: HEADER_NAMES is required")
    endif()

    if(NOT ARGS_OUTPUT_DIR)
        set(ARGS_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
    elseif(NOT IS_ABSOLUTE "${ARGS_OUTPUT_DIR}")
        set(ARGS_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/${ARGS_OUTPUT_DIR}")
    endif()

    set(portable_headers_list)
    foreach(class_name ${ARGS_HEADER_NAMES})
        string(TOLOWER "${class_name}.h" header_file)
        set(portable_header_path "${ARGS_OUTPUT_DIR}/${class_name}")
        file(WRITE "${portable_header_path}" "#include \"${header_file}\"\n")
        list(APPEND portable_headers_list "${portable_header_path}")
    endforeach()

    set(${outfiles} ${portable_headers_list} PARENT_SCOPE)
endfunction()

# Add link optimizations. It make the .so around 33% smaller without any known
# drawback.
include(compiler_settings NO_POLICY_SCOPE)

set(CMAKE_AUTOMOC ON)

if (BUILD_TESTS)
    find_package(Qt6 ${QT_MINIMUM_VERSION} CONFIG REQUIRED Test)
endif()


set(QTXDGX_LIBRARY_NAME "Qt6QuickXdg")
set(QTXDGX_FILE_NAME "qt6quickxdg")

set(QTXDGX_WIDGETS_LIBRARY_NAME "Qt6QuickXdgWidgets")
set(QTXDGX_WIDGETS_FILE_NAME "qt6quickxdgwidgets")

set(QTXDGX_ICONLOADER_LIBRARY_NAME "Qt6QuickXdgIconLoader")
set(QTXDGX_ICONLOADER_FILE_NAME "qt6quickxdgiconloader")
set(QTXDGX_ICONENGINEPLUGIN_LIBRARY_NAME "Qt6QuickXdgIconPlugin")

set(QTXDGX_PKG_CONFIG_DESCRIPTION "Qt6QuickXdg, DeckShell Qt6 implementation of XDG standards")
set(QTXDGX_PKG_CONFIG_REQUIRES "Qt6Core >= ${QT_MINIMUM_VERSION}, Qt6Xml >= ${QT_MINIMUM_VERSION}, Qt6DBus >= ${QT_MINIMUM_VERSION}, Qt6QuickXdgIconLoader = ${QTXDG_VERSION_STRING}")

set(QTXDGX_ICONLOADER_PKG_CONFIG_DESCRIPTION "Qt6QuickXdgIconLoader, DeckShell Qt6 XDG Icon Loader")
set(QTXDGX_ICONLOADER_PKG_CONFIG_REQUIRES "Qt6Gui >= ${QT_MINIMUM_VERSION}, Qt6Svg >= ${QT_MINIMUM_VERSION}")

set(QTXDGX_WIDGETS_PKG_CONFIG_DESCRIPTION "Qt6QuickXdgWidgets, Qt Widgets components for Qt6QuickXdg")
set(QTXDGX_WIDGETS_PKG_CONFIG_REQUIRES "Qt6Core >= ${QT_MINIMUM_VERSION}, Qt6Widgets >= ${QT_MINIMUM_VERSION}, Qt6QuickXdg = ${QTXDG_VERSION_STRING}")

set(QTXDGX_INTREE_INCLUDEDIR "${CMAKE_CURRENT_BINARY_DIR}/InTreeBuild/include")

message(STATUS "Building ${PROJECT_NAME} with Qt ${Qt6Core_VERSION}")

if (QTXDG_INSTALL_DEFAPPS_CONFIG)
    file(GLOB QTXDG_CONFIG_FILES config/*.conf)

    # 使用标准的 CMAKE_INSTALL_SYSCONFDIR
    if(NOT DEFINED CMAKE_INSTALL_SYSCONFDIR)
        set(CMAKE_INSTALL_SYSCONFDIR "/etc" CACHE PATH "System configuration directory")
    endif()

    set(QTXDG_DEFAPPS_CONF_INSTALL_DIR
        "${CMAKE_INSTALL_SYSCONFDIR}/xdg"
        CACHE PATH "Path to qtxdg default apps conf files install dir")
    mark_as_advanced(QTXDG_DEFAPPS_CONF_INSTALL_DIR)
endif()


add_subdirectory(src)

if(BUILD_TESTS)
    enable_testing()
    target_compile_definitions(${QTXDGX_LIBRARY_NAME}
        PRIVATE "QTXDG_TESTS=\"1\""
    )
    add_subdirectory(test)
else()
    message(STATUS "")
    message(STATUS "For building tests use -DBUILD_TESTS=Yes option.")
    message(STATUS "")
endif()

if (BUILD_DEV_UTILS)
    add_subdirectory(util)
endif()

if(BUILD_QML_PLUGIN)
    message(STATUS "")
    message(STATUS "Building QML plugin for Qt6Xdg")
    add_subdirectory(src/qtxdgqml)
    message(STATUS "")
else()
    message(STATUS "")
    message(STATUS "For building QML plugin use -DBUILD_QML_PLUGIN=ON option.")
    message(STATUS "")
endif()

if(BUILD_EXAMPLES)
    message(STATUS "")
    message(STATUS "Building examples")
    add_subdirectory(examples)
    message(STATUS "")
else()
    message(STATUS "")
    message(STATUS "For building examples use -DBUILD_EXAMPLES=ON option.")
    message(STATUS "")
endif()

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${QTXDGX_FILE_NAME}-config.cmake.in"
    "${CMAKE_BINARY_DIR}/${QTXDGX_FILE_NAME}-config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${QTXDGX_FILE_NAME}"
)

write_basic_package_version_file(
    "${CMAKE_BINARY_DIR}/${QTXDGX_FILE_NAME}-config-version.cmake"
    VERSION ${QTXDG_VERSION_STRING}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${QTXDGX_ICONLOADER_FILE_NAME}-config.cmake.in"
    "${CMAKE_BINARY_DIR}/${QTXDGX_ICONLOADER_FILE_NAME}-config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${QTXDGX_ICONLOADER_FILE_NAME}"
)

write_basic_package_version_file(
    "${CMAKE_BINARY_DIR}/${QTXDGX_ICONLOADER_FILE_NAME}-config-version.cmake"
    VERSION ${QTXDG_VERSION_STRING}
    COMPATIBILITY AnyNewerVersion
)

# 生成 pkg-config 文件
configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/Qt6QuickXdg.pc.in"
    "${CMAKE_BINARY_DIR}/${QTXDGX_LIBRARY_NAME}.pc"
    @ONLY
)

configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/Qt6QuickXdgIconLoader.pc.in"
    "${CMAKE_BINARY_DIR}/${QTXDGX_ICONLOADER_LIBRARY_NAME}.pc"
    @ONLY
)

if(BUILD_WIDGETS_LIB)
    configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/cmake/${QTXDGX_WIDGETS_FILE_NAME}-config.cmake.in"
        "${CMAKE_BINARY_DIR}/${QTXDGX_WIDGETS_FILE_NAME}-config.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${QTXDGX_WIDGETS_FILE_NAME}"
    )

    write_basic_package_version_file(
        "${CMAKE_BINARY_DIR}/${QTXDGX_WIDGETS_FILE_NAME}-config-version.cmake"
        VERSION ${QTXDG_VERSION_STRING}
        COMPATIBILITY AnyNewerVersion
    )

    # 生成 Widgets pkg-config 文件
    configure_file(
        "${PROJECT_SOURCE_DIR}/cmake/Qt6QuickXdgWidgets.pc.in"
        "${CMAKE_BINARY_DIR}/${QTXDGX_WIDGETS_LIBRARY_NAME}.pc"
        @ONLY
    )
endif()

install(FILES
    "${CMAKE_BINARY_DIR}/${QTXDGX_FILE_NAME}-config.cmake"
    "${CMAKE_BINARY_DIR}/${QTXDGX_FILE_NAME}-config-version.cmake"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${QTXDGX_FILE_NAME}"
    COMPONENT Devel
)

install(FILES
    "${CMAKE_BINARY_DIR}/${QTXDGX_ICONLOADER_FILE_NAME}-config.cmake"
    "${CMAKE_BINARY_DIR}/${QTXDGX_ICONLOADER_FILE_NAME}-config-version.cmake"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${QTXDGX_ICONLOADER_FILE_NAME}"
    COMPONENT Devel
)

install(EXPORT
    "${QTXDGX_FILE_NAME}-targets"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${QTXDGX_FILE_NAME}"
    FILE "${QTXDGX_FILE_NAME}-targets.cmake"
    COMPONENT Devel
)

install(EXPORT
    "${QTXDGX_ICONLOADER_FILE_NAME}-targets"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${QTXDGX_ICONLOADER_FILE_NAME}"
    FILE "${QTXDGX_ICONLOADER_FILE_NAME}-targets.cmake"
    COMPONENT Devel
)

if(BUILD_WIDGETS_LIB)
    install(FILES
        "${CMAKE_BINARY_DIR}/${QTXDGX_WIDGETS_FILE_NAME}-config.cmake"
        "${CMAKE_BINARY_DIR}/${QTXDGX_WIDGETS_FILE_NAME}-config-version.cmake"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${QTXDGX_WIDGETS_FILE_NAME}"
        COMPONENT Devel
    )

    install(EXPORT
        "${QTXDGX_WIDGETS_FILE_NAME}-targets"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${QTXDGX_WIDGETS_FILE_NAME}"
        FILE "${QTXDGX_WIDGETS_FILE_NAME}-targets.cmake"
        COMPONENT Devel
    )
endif()

if (QTXDG_INSTALL_DEFAPPS_CONFIG)
    install(FILES ${QTXDG_CONFIG_FILES}
        DESTINATION "${QTXDG_DEFAPPS_CONF_INSTALL_DIR}"
        COMPONENT Runtime
    )
endif()

# 安装 pkg-config 文件
# FreeBSD 使用不同的 pkgconfig 路径
if(${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
    set(PKGCONFIG_INSTALL_DIR "libdata/pkgconfig")
else()
    set(PKGCONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
endif()

install(FILES
    "${CMAKE_BINARY_DIR}/${QTXDGX_LIBRARY_NAME}.pc"
    "${CMAKE_BINARY_DIR}/${QTXDGX_ICONLOADER_LIBRARY_NAME}.pc"
    DESTINATION "${PKGCONFIG_INSTALL_DIR}"
    COMPONENT Devel
)

if(BUILD_WIDGETS_LIB)
    install(FILES
        "${CMAKE_BINARY_DIR}/${QTXDGX_WIDGETS_LIBRARY_NAME}.pc"
        DESTINATION "${PKGCONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
endif()

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

#add_custom_target(uninstall
#    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
