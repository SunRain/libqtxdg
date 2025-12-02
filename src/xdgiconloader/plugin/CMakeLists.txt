set(xdgiconengineplugin_CPP_FILES
    xdgiconengineplugin.cpp
)

add_library(${QTXDGX_ICONENGINEPLUGIN_LIBRARY_NAME} MODULE
    ${xdgiconengineplugin_CPP_FILES}
)

target_compile_definitions(${QTXDGX_ICONENGINEPLUGIN_LIBRARY_NAME}
    PRIVATE
        "QT_NO_KEYWORDS"
)
target_link_libraries(${QTXDGX_ICONENGINEPLUGIN_LIBRARY_NAME}
    PRIVATE
        Qt6::GuiPrivate
    PUBLIC
        Qt6::Gui
        "${QTXDGX_ICONLOADER_LIBRARY_NAME}"
)

target_include_directories("${QTXDGX_ICONENGINEPLUGIN_LIBRARY_NAME}"
    PRIVATE
        "${Qt6Gui_PRIVATE_INCLUDE_DIRS}"
)

mark_as_advanced(QTXDGX_ICONENGINEPLUGIN_INSTALL_PATH)

if (NOT DEFINED QTXDGX_ICONENGINEPLUGIN_INSTALL_PATH)
    # 使用 Qt6::qmake 查询 Qt 插件目录
    find_package(Qt6 COMPONENTS CoreTools QUIET CONFIG)
    if (TARGET Qt6::qtpaths)
        get_target_property(_qtpaths_executable Qt6::qtpaths LOCATION)
        execute_process(
            COMMAND ${_qtpaths_executable} --query QT_INSTALL_PLUGINS
            RESULT_VARIABLE _result
            OUTPUT_VARIABLE _QT_PLUGINS_DIR
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if (NOT _result EQUAL 0 OR NOT _QT_PLUGINS_DIR)
            message(FATAL_ERROR "Failed to query Qt6 plugin directory")
        endif()
    else()
        message(FATAL_ERROR "Qt6::qtpaths not found, cannot determine plugin directory")
    endif()

    # 使用相对于 CMAKE_INSTALL_PREFIX 的路径
    # 从系统 Qt 路径中提取相对路径部分（去掉前导的 /）
    string(REGEX REPLACE "^/" "" _QT_PLUGINS_DIR_RELATIVE "${_QT_PLUGINS_DIR}")
    set(QTXDGX_ICONENGINEPLUGIN_INSTALL_PATH "${_QT_PLUGINS_DIR_RELATIVE}/iconengines")
endif()

message(STATUS "XdgIconEnginePlugin will be installed into: ${CMAKE_INSTALL_PREFIX}/${QTXDGX_ICONENGINEPLUGIN_INSTALL_PATH}")

install(TARGETS
    "${QTXDGX_ICONENGINEPLUGIN_LIBRARY_NAME}" DESTINATION "${QTXDGX_ICONENGINEPLUGIN_INSTALL_PATH}"
    COMPONENT Runtime
)

