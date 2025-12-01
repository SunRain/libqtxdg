set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(QTXDGWIDGETS_LIBRARIES Qt6::Widgets Qt6::Xml Qt6::DBus)

set(libqtxdgwidgets_PUBLIC_H_FILES
    xdgaction.h
    xdgmenuwidget.h
)

set(libqtxdgwidgets_PUBLIC_CLASSES
    XdgAction
    XdgMenuWidget
)

set(libqtxdgwidgets_CPP_FILES
    xdgaction.cpp
    xdgmenuwidget.cpp
)

add_library(${QTXDGX_WIDGETS_LIBRARY_NAME} SHARED
    ${libqtxdgwidgets_PUBLIC_H_FILES}
    ${libqtxdgwidgets_CPP_FILES}
)

target_link_libraries(${QTXDGX_WIDGETS_LIBRARY_NAME}
    PUBLIC
        ${QTXDGWIDGETS_LIBRARIES}
        ${QTXDGX_LIBRARY_NAME}
)

set_target_properties(${QTXDGX_WIDGETS_LIBRARY_NAME} PROPERTIES
  VERSION ${QTXDG_VERSION_STRING}
  SOVERSION ${QTXDG_MAJOR_VERSION}
)

target_compile_definitions(${QTXDGX_WIDGETS_LIBRARY_NAME}
    PRIVATE
        "QTXDGWIDGETS_COMPILATION=\"1\""
        "QTXDG_VERSION=\"${QTXDG_VERSION_STRING}\""
        "QT_NO_KEYWORDS"
)

target_include_directories(${QTXDGX_WIDGETS_LIBRARY_NAME}
    INTERFACE
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${QTXDGX_WIDGETS_FILE_NAME}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
        "$<BUILD_INTERFACE:${QTXDGX_INTREE_INCLUDEDIR}/${QTXDGX_WIDGETS_FILE_NAME}>"
        "$<BUILD_INTERFACE:${QTXDGX_INTREE_INCLUDEDIR}>"
)

# create the portable headers
qtxdg_create_portable_headers(libqtxdgwidgets_PORTABLE_HEADERS
    HEADER_NAMES ${libqtxdgwidgets_PUBLIC_CLASSES}
    OUTPUT_DIR "${QTXDGX_INTREE_INCLUDEDIR}/${QTXDGX_WIDGETS_FILE_NAME}"
)

# Copy public headers (in tree building)
foreach(h ${libqtxdgwidgets_PUBLIC_H_FILES})
    get_filename_component(bh ${h} NAME)
    configure_file(${h} "${QTXDGX_INTREE_INCLUDEDIR}/${QTXDGX_WIDGETS_FILE_NAME}/${bh}" COPYONLY)
endforeach()

install(TARGETS
    ${QTXDGX_WIDGETS_LIBRARY_NAME} DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    EXPORT "${QTXDGX_WIDGETS_FILE_NAME}-targets"
    COMPONENT Runtime
)

install(FILES
    ${libqtxdgwidgets_PUBLIC_H_FILES}
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${QTXDGX_WIDGETS_FILE_NAME}"
    COMPONENT Devel
)

install(FILES
    ${libqtxdgwidgets_PORTABLE_HEADERS}
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${QTXDGX_WIDGETS_FILE_NAME}"
    COMPONENT Devel
)
