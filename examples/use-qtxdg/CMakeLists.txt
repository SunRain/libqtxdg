cmake_minimum_required(VERSION 3.18.0 FATAL_ERROR)
# CMP0000: Call the cmake_minimum_required() command at the beginning of the top-level
# CMakeLists.txt file even before calling the project() command.
# The cmake_minimum_required(VERSION) command implicitly invokes the cmake_policy(VERSION)
# command to specify that the current project code is written for the given range of CMake
# versions.
project(use-qtxdg)

# When building as part of libqtxdg, Qt6Xdg target is already available
# When building standalone, find the installed package
if(NOT TARGET Qt6Xdg)
    find_package(Qt6Xdg REQUIRED)
endif()

add_executable(use-qtxdg main.cpp)
target_link_libraries(use-qtxdg Qt6Xdg)

# Install target
install(TARGETS use-qtxdg
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    COMPONENT Examples
)
