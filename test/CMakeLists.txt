remove_definitions(
    -DQT_USE_QSTRINGBUILDER
    -DQT_NO_CAST_FROM_ASCII
)

add_definitions(
    -DQT_NO_KEYWORDS
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

macro(qtxdg_add_test)
    foreach(_testname ${ARGN})
        add_executable(${_testname} ${_testname}.cpp)
        target_link_libraries(${_testname} Qt6::Test ${QTXDGX_LIBRARY_NAME})
        target_include_directories(${_testname}
            PRIVATE "${PROJECT_SOURCE_DIR}/src/qtxdg"
        )
        add_test(NAME ${_testname} COMMAND ${_testname})
    endforeach()
endmacro()

set_property(DIRECTORY APPEND
    PROPERTY COMPILE_DEFINITIONS "QTXDG_BUILDING_TESTS=\"1\""
)

qtxdg_add_test(
    qtxdg_test
    tst_xdgdirs
    tst_xdgdesktopfile
)

# QML wrapper tests - compile sources directly to avoid linking issues
if(BUILD_QML_PLUGIN)
    add_executable(tst_xdgmimewrapper
        tst_xdgmimewrapper.cpp
        ../src/qtxdgqml/xdgmimetypewrapper.cpp
    )
    target_link_libraries(tst_xdgmimewrapper
        Qt6::Test
        ${QTXDGX_LIBRARY_NAME}
    )
    target_include_directories(tst_xdgmimewrapper
        PRIVATE "${PROJECT_SOURCE_DIR}/src/qtxdg"
        PRIVATE "${PROJECT_SOURCE_DIR}/src/qtxdgqml"
    )
    add_test(NAME tst_xdgmimewrapper COMMAND tst_xdgmimewrapper)

    add_executable(tst_xdgmimeappswrapper
        tst_xdgmimeappswrapper.cpp
        ../src/qtxdgqml/xdgmimeappswrapper.cpp
    )
    target_link_libraries(tst_xdgmimeappswrapper
        Qt6::Test
        ${QTXDGX_LIBRARY_NAME}
    )
    target_include_directories(tst_xdgmimeappswrapper
        PRIVATE "${PROJECT_SOURCE_DIR}/src/qtxdg"
        PRIVATE "${PROJECT_SOURCE_DIR}/src/qtxdgqml"
    )
    add_test(NAME tst_xdgmimeappswrapper COMMAND tst_xdgmimeappswrapper)

    add_executable(tst_xdgmenuwrapper
        tst_xdgmenuwrapper.cpp
        ../src/qtxdgqml/xdgmenuwrapper.cpp
    )
    target_link_libraries(tst_xdgmenuwrapper
        Qt6::Test
        ${QTXDGX_LIBRARY_NAME}
    )
    target_include_directories(tst_xdgmenuwrapper
        PRIVATE "${PROJECT_SOURCE_DIR}/src/qtxdg"
        PRIVATE "${PROJECT_SOURCE_DIR}/src/qtxdgqml"
    )
    add_test(NAME tst_xdgmenuwrapper COMMAND tst_xdgmenuwrapper)

    # XdgMenuTreeModel test - compile sources directly with proper dependencies
    add_executable(tst_xdgmenutreemodel
        tst_xdgmenutreemodel.cpp
        ../src/qtxdgqml/xdgmenutreemodel.cpp
        ../src/qtxdgqml/xdgmenutreemodel.h
    )
    target_link_libraries(tst_xdgmenutreemodel
        Qt6::Test
        Qt6::Concurrent
        ${QTXDGX_LIBRARY_NAME}
    )
    target_include_directories(tst_xdgmenutreemodel
        PRIVATE "${PROJECT_SOURCE_DIR}/src/qtxdg"
        PRIVATE "${PROJECT_SOURCE_DIR}/src/qtxdgqml"
    )
    set_target_properties(tst_xdgmenutreemodel PROPERTIES
        AUTOMOC ON
    )
    add_test(NAME tst_xdgmenutreemodel COMMAND tst_xdgmenutreemodel)

    # XdgMenuTreeModel benchmark - separate from unit tests
    add_executable(bench_xdgmenutreemodel
        bench_xdgmenutreemodel.cpp
        ../src/qtxdgqml/xdgmenutreemodel.cpp
        ../src/qtxdgqml/xdgmenutreemodel.h
    )
    target_link_libraries(bench_xdgmenutreemodel
        Qt6::Test
        Qt6::Concurrent
        ${QTXDGX_LIBRARY_NAME}
    )
    target_include_directories(bench_xdgmenutreemodel
        PRIVATE "${PROJECT_SOURCE_DIR}/src/qtxdg"
        PRIVATE "${PROJECT_SOURCE_DIR}/src/qtxdgqml"
    )
    set_target_properties(bench_xdgmenutreemodel PROPERTIES
        AUTOMOC ON
    )
    # Note: Benchmarks are not added to CTest, run manually

    # XdgApplicationsModel test - compile sources directly to test SearchMode
    add_executable(tst_xdgapplicationsmodel
        tst_xdgapplicationsmodel.cpp
        ../src/qtxdgqml/xdgapplicationsmodel.cpp
        ../src/qtxdgqml/xdgapplicationsmodel.h
        ../src/qtxdgqml/xdgsimplelistmodel.cpp
        ../src/qtxdgqml/xdgsimplelistmodel.h
    )
    target_link_libraries(tst_xdgapplicationsmodel
        Qt6::Test
        Qt6::Concurrent
        ${QTXDGX_LIBRARY_NAME}
    )
    target_include_directories(tst_xdgapplicationsmodel
        PRIVATE "${PROJECT_SOURCE_DIR}/src/qtxdg"
        PRIVATE "${PROJECT_SOURCE_DIR}/src/qtxdgqml"
    )
    set_target_properties(tst_xdgapplicationsmodel PROPERTIES
        AUTOMOC ON
    )
    add_test(NAME tst_xdgapplicationsmodel COMMAND tst_xdgapplicationsmodel)
endif()

